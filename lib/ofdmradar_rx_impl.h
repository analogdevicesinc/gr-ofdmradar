/* -*- c++ -*- */
/*
 * Copyright 2021 Analog Devices, Inc.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef INCLUDED_OFDMRADAR_OFDMRADAR_RX_IMPL_H
#define INCLUDED_OFDMRADAR_OFDMRADAR_RX_IMPL_H

#include "ofdmradar_impl.h"

#include <ofdmradar/ofdmradar_rx.h>

#include <pmt/pmt.h>

namespace gr {
namespace ofdmradar {

class ofdmradar_rx_impl : public ofdmradar_rx, ofdmradar_shared
{
private:
    callback_t d_callback;
    pmt::pmt_t d_len_tag_key;
    std::vector<gr_complex> d_tx_symbols;
    std::vector<tag_t> d_tags;
    std::vector<gr_complex> d_frame_buffer;
    fftwf_plan d_doppler_fft_plan;

public:
    ofdmradar_rx_impl(ofdmradar_params::sptr ofdm_params,
                      callback_t &&callback,
                      const std::string &len_tag_key);
    ~ofdmradar_rx_impl();

    // Where all the action really happens
    int work(int noutput_items,
             gr_vector_const_void_star &input_items,
             gr_vector_void_star &output_items);
};

} // namespace ofdmradar
} // namespace gr

#endif /* INCLUDED_OFDMRADAR_OFDMRADAR_RX_IMPL_H */
