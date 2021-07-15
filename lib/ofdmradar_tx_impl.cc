/* -*- c++ -*- */
/*
 * Copyright 2021 Analog Devices, Inc.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "ofdmradar_tx_impl.h"

#include <gnuradio/io_signature.h>

#include <algorithm>
#include <cstring>

namespace gr {
namespace ofdmradar {

ofdmradar_tx::sptr ofdmradar_tx::make(ofdmradar_params::sptr ofdm_params,
                                      const std::string &len_tag_key)
{
    return gnuradio::make_block_sptr<ofdmradar_tx_impl>(ofdm_params, len_tag_key);
}

/*
 * The private constructor
 */
ofdmradar_tx_impl::ofdmradar_tx_impl(ofdmradar_params::sptr ofdm_params,
                                     const std::string &len_tag_key)
    : gr::sync_block("ofdmradar_tx",
                     gr::io_signature::make(0, 0, 0),
                     gr::io_signature::make(1, 1, sizeof(gr_complex))),
      ofdmradar_shared(ofdm_params),
      d_frame_buffer(ofdm_params->frame_length()),
      d_len_tag_key(pmt::intern(len_tag_key))
{
    generate_frame(d_frame_buffer.data());
    set_output_multiple(0x1000);
}

void ofdmradar_tx_impl::generate_symbol(gr_complex *out)
{
    generate_tx_symbols(d_fft_gr_in);

    fftwf_execute(d_ifft_plan);

    for (unsigned int i = 0;
         i < d_ofdm_params->carriers() && i < d_ofdm_params->cyclic_prefix_length();
         i++)
        out[i] = d_fft_gr_out[d_ofdm_params->carriers() -
                              d_ofdm_params->cyclic_prefix_length() + i];
    for (unsigned int i = 0; i < d_ofdm_params->carriers(); i++)
        out[d_ofdm_params->cyclic_prefix_length() + i] = d_fft_gr_out[i];
}

void ofdmradar_tx_impl::generate_frame(gr_complex *out)
{
    // d_generator.seed(d_ofdm_params->seed());
    for (unsigned int i = 0; i < d_ofdm_params->symbols(); i++)
        generate_symbol(&out[i * d_ofdm_params->symbol_length()]);
}

/*
 * Our virtual destructor.
 */
ofdmradar_tx_impl::~ofdmradar_tx_impl() {}

int ofdmradar_tx_impl::work(int noutput_items,
                            gr_vector_const_void_star &input_items,
                            gr_vector_void_star &output_items)
{
    gr_complex *out = reinterpret_cast<gr_complex *>(output_items[0]);

    if (noutput_items < 0)
        throw std::runtime_error("Negative noutput_items?!");

    int items = std::min<int>(noutput_items, d_frame_buffer.size() - d_running_idx);

    // Beginning of new packet?
    if (!d_running_idx) {
        add_item_tag(0,
                     nitems_written(0),
                     d_len_tag_key,
                     pmt::from_long(d_frame_buffer.size()));
    }
    std::memcpy(out, &d_frame_buffer[d_running_idx], sizeof(gr_complex) * items);

    d_running_idx += items;
    if (d_running_idx == d_frame_buffer.size())
        d_running_idx = 0;

    return items;
}

} /* namespace ofdmradar */
} /* namespace gr */
