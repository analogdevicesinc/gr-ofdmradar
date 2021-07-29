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
    pmt::pmt_t d_len_tag_key;
    size_t d_out_size;
    std::vector<gr_complex> d_tx_symbols;
    size_t d_symbol_idx = 0;
    size_t d_wr_symbol_idx = 0;
    size_t d_carrier_idx = 0;
    std::vector<gr_complex> d_frame_buffer;
    std::vector<tag_t> d_tags;
    fftwf_plan d_doppler_fft_plan;
    fftwf_plan d_peri_c_ifft_plan;
    size_t d_buffer_size;
    size_t d_total_consumed = 0;
    std::vector<float> d_window_carriers;
    std::vector<float> d_window_symbols;

public:
    ofdmradar_rx_impl(ofdmradar_params::sptr ofdm_params,
                      const std::string &len_tag_key,
                      size_t buffer_size);
    ~ofdmradar_rx_impl();

    int general_work(int noutput_items,
                     gr_vector_int &ninput_items,
                     gr_vector_const_void_star &input_items,
                     gr_vector_void_star &output_items) override;

    void forecast(int noutput_items, gr_vector_int &ninput_items_required) override;
};

} // namespace ofdmradar
} // namespace gr

#endif /* INCLUDED_OFDMRADAR_OFDMRADAR_RX_IMPL_H */
