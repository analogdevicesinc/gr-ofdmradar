/* -*- c++ -*- */
/*
 * Copyright 2021 Analog Devices, Inc.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef INCLUDED_OFDMRADAR_OFDMRADAR_TX_IMPL_H
#define INCLUDED_OFDMRADAR_OFDMRADAR_TX_IMPL_H

#include "ofdmradar_impl.h"

#include <ofdmradar/ofdmradar_tx.h>

#include <pmt/pmt.h>

#include <vector>

namespace gr {
namespace ofdmradar {

class ofdmradar_tx_impl : public ofdmradar_tx, ofdmradar_shared
{
    const pmt::pmt_t d_len_tag_key;
    std::vector<gr_complex> d_frame_buffer;

    void generate_symbol(gr_complex *out);
    void generate_frame(gr_complex *out);
public:
    ofdmradar_tx_impl(ofdmradar_params::sptr ofdm_params,
                      const std::string &len_tag_key);
    ~ofdmradar_tx_impl();

    // Where all the action really happens
    int work(int noutput_items,
             gr_vector_const_void_star &input_items,
             gr_vector_void_star &output_items);
};

} // namespace ofdmradar
} // namespace gr

#endif /* INCLUDED_OFDMRADAR_OFDMRADAR_TX_IMPL_H */
