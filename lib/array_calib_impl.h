/* -*- c++ -*- */
/*
 * Copyright 2021 Analog Devices Inc.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef INCLUDED_OFDMRADAR_ARRAY_CALIB_IMPL_H
#define INCLUDED_OFDMRADAR_ARRAY_CALIB_IMPL_H

#include <ofdmradar/array_calib.h>

#include <pmt/pmt.h>

#include <atomic>
#include <vector>

namespace gr {
namespace ofdmradar {

class array_calib_impl : public array_calib
{
private:
    int d_array_size;
    int d_targets;
    float d_pilot_angle;
    pmt::pmt_t d_calib_msg_port_id;
    pmt::pmt_t d_trigger_msg_port_id;
    std::vector<gr_complex> d_steering_vector;
    std::atomic<bool> d_calib_requested;
    std::vector<gr_complex> d_gamma;

public:
    array_calib_impl(int array_size, int targets, float pilot_angle);
    ~array_calib_impl();

    // Where all the action really happens
    int work(int noutput_items,
             gr_vector_const_void_star &input_items,
             gr_vector_void_star &output_items);
};

} // namespace ofdmradar
} // namespace gr

#endif /* INCLUDED_OFDMRADAR_ARRAY_CALIB_IMPL_H */
