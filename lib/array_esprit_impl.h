/* -*- c++ -*- */
/*
 * Copyright 2021 Analog Devices Inc.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef INCLUDED_OFDMRADAR_ARRAY_ESPRIT_IMPL_H
#define INCLUDED_OFDMRADAR_ARRAY_ESPRIT_IMPL_H

#include <ofdmradar/array_esprit.h>

namespace gr {
namespace ofdmradar {

class array_esprit_impl : public array_esprit
{
private:
    // Nothing to declare in this block.

public:
    array_esprit_impl();
    ~array_esprit_impl();

    // Where all the action really happens
    int work(int noutput_items,
             gr_vector_const_void_star &input_items,
             gr_vector_void_star &output_items);
};

} // namespace ofdmradar
} // namespace gr

#endif /* INCLUDED_OFDMRADAR_ARRAY_ESPRIT_IMPL_H */
