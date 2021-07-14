/* -*- c++ -*- */
/*
 * Copyright 2021 Analog Devices, Inc.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "ofdmradar_rx_impl.h"
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
      d_out_size(ofdm_params->carriers() * ofdm_params->symbols()),
      d_tx_symbols(d_out_size),
      d_buffer_size(buffer_size)
{
    if (buffer_size == (size_t)-1LL)
        d_buffer_size = ofdm_params->frame_length();

    this->set_output_multiple(d_out_size);

    d_doppler_fft_plan = fftwf_plan_dft_1d(
        d_ofdm_params->symbols(), d_fft_in, d_fft_out, FFTW_FORWARD, FFTW_MEASURE);

    // Generate reference data
    for (unsigned int i_s = 0; i_s < ofdm_params->symbols(); i_s++) {
        unsigned int offset = i_s * ofdm_params->carriers();
        generate_tx_symbols(&d_tx_symbols[offset]);
    }
}

ofdmradar_rx_impl::~ofdmradar_rx_impl() {}

void ofdmradar_rx_impl::forecast(int noutput_items, gr_vector_int &nitemsreq)
{
    nitemsreq[0] = (int)(noutput_items / d_out_size) * (int)d_buffer_size;
}

int ofdmradar_rx_impl::general_work(int noutput_items,
                                    gr_vector_int &ninput_items,
                                    gr_vector_const_void_star &input_items,
                                    gr_vector_void_star &output_items)
{
    if (static_cast<size_t>(noutput_items) < d_out_size ||
        static_cast<size_t>(ninput_items[0]) < d_buffer_size)
        throw std::runtime_error("ofdmradar_rx: Received undersized buffer!");

    return work(noutput_items, input_items, output_items);
}

int ofdmradar_rx_impl::work(int noutput_items,
                            gr_vector_const_void_star &input_items,
                            gr_vector_void_star &output_items)
{
    const gr_complex *in = reinterpret_cast<const gr_complex *>(input_items[0]);
    gr_complex *out = reinterpret_cast<gr_complex *>(output_items[0]);
    const auto n = d_ofdm_params->carriers();
    const auto m = d_ofdm_params->symbols();
    const auto cpl = d_ofdm_params->cyclic_prefix_length();

    // Remove cyclic prefix when copying
    for (int i = 0; i < m; i++) {
        // Run FFT on symbol
        std::memcpy(d_fft_gr_in, &in[i * (n + cpl) + cpl], sizeof(gr_complex) * n);
        fftwf_execute(d_fft_plan);
        std::memcpy(&out[i * n], d_fft_gr_out, sizeof(gr_complex) * n);
    }

    // Remove tx symbol influence
    for (int i_s = 0; i_s < m; i_s++) {
        for (int i_c = 0; i_c < n; i_c++) {
            if (d_ofdm_params->carrier_mask()[i_c])
                out[i_s * n + i_c] /= d_tx_symbols[i_s * n + i_c];
            else
                out[i_s * n + i_c] = 0;
        }
    }

    // Transform back to time domain along carrier axis
    for (int i = 0; i < m; i++) {
        // Run FFT on symbol
        std::memcpy(d_fft_gr_in, &out[i * n], sizeof(gr_complex) * n);
        fftwf_execute(d_ifft_plan);
        std::memcpy(&out[i * n], d_fft_gr_out, sizeof(gr_complex) * n);
    }

    // Transform to doppler domain along symbol axis
    for (int i = 0; i < n; i++) {
        for (int i_s = 0; i_s < m; i_s++)
            d_fft_gr_in[i_s] = out[i_s * n + i];

        fftwf_execute(d_doppler_fft_plan);

        for (int i_s = 0; i_s < m; i_s++)
            out[i_s * n + i] = d_fft_gr_out[i_s];
    }

    // Tell runtime system how many items we consumed
    consume(0, d_buffer_size);
    return d_out_size;
}

} /* namespace ofdmradar */
} /* namespace gr */
