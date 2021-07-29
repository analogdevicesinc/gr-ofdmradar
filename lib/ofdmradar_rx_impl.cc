/* -*- c++ -*- */
/*
 * Copyright 2021 Analog Devices, Inc.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "ofdmradar_rx_impl.h"

#include <gnuradio/fft/window.h>
#include <gnuradio/io_signature.h>

#include <cmath>
#include <cstring>

namespace gr {
namespace ofdmradar {

ofdmradar_rx::sptr ofdmradar_rx::make(ofdmradar_params::sptr ofdm_params,
                                      const std::string &len_tag_key,
                                      size_t buffer_size)
{
    return gnuradio::make_block_sptr<ofdmradar_rx_impl>(
        ofdm_params, len_tag_key, buffer_size);
}

namespace {

// Only to be used for even sizes
unsigned int fftshift(unsigned int i, unsigned int N)
{
    unsigned int half = N >> 1;

    if (i < half)
        return i + half;
    return i - half;
}

unsigned int pad_spectrum(unsigned int i, unsigned int from, unsigned int to)
{
    // 0, 1, 2, 3,                             -4, -3, -2, -1
    // 0, 1, 2, 3, 4, 5, 6, 7, -8, -7, -6, -5, -4, -3, -2, -1

    unsigned int half = from / 2;

    if (i >= to)
        throw std::runtime_error("i >= to?!");

    if (i < half)
        return i;

    return to - from + i;
}

} // namespace


/*
 * The private constructor
 */
ofdmradar_rx_impl::ofdmradar_rx_impl(ofdmradar_params::sptr ofdm_params,
                                     const std::string &len_tag_key,
                                     size_t buffer_size)
    : gr::block("ofdmradar_rx",
                gr::io_signature::make(1, 1, sizeof(gr_complex)),
                gr::io_signature::make(1, 1, sizeof(gr_complex))),
      ofdmradar_shared(ofdm_params),
      d_len_tag_key(pmt::intern(len_tag_key)),
      d_out_size(ofdm_params->peri_length()),
      d_tx_symbols(ofdm_params->carriers() * ofdm_params->symbols()),
      d_frame_buffer(d_out_size),
      d_buffer_size(buffer_size),
      d_window_carriers(ofdm_params->carriers()),
      d_window_symbols(ofdm_params->symbols())
{
    if (buffer_size == (size_t)-1LL)
        d_buffer_size = ofdm_params->frame_length();

    this->set_output_multiple(ofdm_params->peri_carriers());

    d_doppler_fft_plan = fftwf_plan_dft_1d(
        d_ofdm_params->peri_symbols(), d_fft_in, d_fft_out, FFTW_FORWARD, FFTW_MEASURE);

    d_peri_c_ifft_plan = fftwf_plan_dft_1d(
        d_ofdm_params->peri_carriers(), d_fft_in, d_fft_out, FFTW_BACKWARD, FFTW_MEASURE);

    // Generate reference data
    for (unsigned int i_s = 0; i_s < ofdm_params->symbols(); i_s++) {
        unsigned int offset = i_s * ofdm_params->carriers();
        generate_tx_symbols(&d_tx_symbols[offset]);
    }

    auto c_window = ofdm_params->window(ofdm_params->carriers());
    auto s_window = ofdm_params->window(ofdm_params->symbols());
    float energy = 0;
    for (unsigned int i = 0; i < ofdm_params->carriers(); i++) {
        for (unsigned int j = 0; j < ofdm_params->symbols(); j++) {
            float w = c_window[i] * s_window[j];
            energy += w * w;
        }
    }

    float norm =
        1.0f /
        std::sqrt(std::sqrt(energy / (ofdm_params->carriers() * ofdm_params->symbols())));

    for (unsigned int i = 0; i < c_window.size(); i++)
        this->d_window_carriers[i] =
            c_window[fftshift(i, ofdm_params->carriers())] * norm;

    for (unsigned int i = 0; i < s_window.size(); i++)
        this->d_window_symbols[i] = s_window[i] * norm;
}

ofdmradar_rx_impl::~ofdmradar_rx_impl()
{
    fftwf_destroy_plan(d_doppler_fft_plan);
    fftwf_destroy_plan(d_peri_c_ifft_plan);
}

void ofdmradar_rx_impl::forecast(int noutput_items, gr_vector_int &nitemsreq)
{
    nitemsreq[0] = std::min(0x1000UL, d_buffer_size - d_total_consumed);
}

int ofdmradar_rx_impl::general_work(int noutput_items,
                                    gr_vector_int &ninput_items,
                                    gr_vector_const_void_star &input_items,
                                    gr_vector_void_star &output_items)
{
    const gr_complex *const in = reinterpret_cast<const gr_complex *>(input_items[0]);
    gr_complex *const out = reinterpret_cast<gr_complex *>(output_items[0]);
    const auto n = d_ofdm_params->carriers();
    const auto peri_n = d_ofdm_params->peri_carriers();
    const auto m = d_ofdm_params->symbols();
    const auto peri_m = d_ofdm_params->peri_symbols();
    const auto cpl = d_ofdm_params->cyclic_prefix_length();
    const int in_items = ninput_items[0];

    int consumed = 0;

    for (; d_symbol_idx < m; d_symbol_idx++) {
        if (in_items - consumed < n + cpl) {
            // Not enough samples the next fft
            consume(0, consumed);
            return 0;
        }

        std::memcpy(d_fft_gr_in, &in[consumed + cpl / 2], sizeof(gr_complex) * n);
        consumed += n + cpl;
        d_total_consumed += n + cpl;

        fftwf_execute(d_fft_plan);

        std::memset(d_fft_in, 0, sizeof(gr_complex) * peri_n);
        // Divide out TX symbols
        for (int i_c = 0; i_c < n; i_c++) {
            unsigned int idx = pad_spectrum(i_c, n, peri_n);
            if (d_ofdm_params->carrier_mask()[i_c])
                d_fft_gr_in[idx] = d_window_carriers[i_c] * d_fft_gr_out[i_c] /
                                   d_tx_symbols[d_symbol_idx * n + i_c];
            else
                d_fft_gr_in[idx] = 0;
        }

        // Transform back to obtain channel response
        fftwf_execute(d_peri_c_ifft_plan);

        // Store in heap buffer
        std::memcpy(&d_frame_buffer[d_symbol_idx * peri_n],
                    d_fft_gr_out,
                    sizeof(gr_complex) * peri_n);
    }
    consume(0, consumed);

    // Transform to doppler domain along symbol axis
    for (; d_carrier_idx < peri_n; d_carrier_idx++) {
        std::memset(d_fft_in, 0, sizeof(gr_complex) * peri_m);
        for (int i_s = 0; i_s < m; i_s++)
            d_fft_gr_in[i_s] =
                d_frame_buffer[i_s * peri_n + d_carrier_idx] * d_window_symbols[i_s];

        fftwf_execute(d_doppler_fft_plan);

        for (int i_s = 0; i_s < peri_m; i_s++)
            d_frame_buffer[i_s * peri_n + d_carrier_idx] =
                d_fft_gr_out[i_s] /
                static_cast<float>(d_ofdm_params->symbols() * d_ofdm_params->carriers());
    }

    int produced = 0;
    for (; d_wr_symbol_idx < peri_m; d_wr_symbol_idx++) {
        if (noutput_items - produced < peri_n)
            return produced;

        std::memcpy(&out[produced],
                    &d_frame_buffer[d_wr_symbol_idx * peri_n],
                    sizeof(gr_complex) * peri_n);

        produced += peri_n;
    }

    while (d_total_consumed < d_buffer_size) {
        if (in_items - consumed == 0)
            return produced; // Need more input

        // Consume remaining items
        consume(0, in_items - consumed);
        d_total_consumed += in_items - consumed;
        consumed = in_items;
    }

    d_symbol_idx = 0;
    d_carrier_idx = 0;
    d_wr_symbol_idx = 0;
    d_total_consumed = 0;
    return produced;
}

} /* namespace ofdmradar */
} /* namespace gr */
