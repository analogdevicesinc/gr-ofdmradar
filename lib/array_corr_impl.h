/* -*- c++ -*- */
/*
 * Copyright 2021 Analog Devices Inc.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef INCLUDED_OFDMRADAR_ARRAY_CORR_IMPL_H
#define INCLUDED_OFDMRADAR_ARRAY_CORR_IMPL_H

#include <ofdmradar/array_corr.h>

namespace gr {
namespace ofdmradar {

class array_corr_impl : public array_corr
{
private:
    const int d_samples;
    const int d_array_size;

public:
    array_corr_impl(int array_size, int samples);
    ~array_corr_impl();

    void forecast(int noutput_items, gr_vector_int &ninput_items_required);

    int general_work(int noutput_items,
                     gr_vector_int &ninput_items,
                     gr_vector_const_void_star &input_items,
                     gr_vector_void_star &output_items);
};

} // namespace ofdmradar
} // namespace gr

#endif /* INCLUDED_OFDMRADAR_ARRAY_CORR_IMPL_H */
