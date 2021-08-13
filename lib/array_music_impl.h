/* -*- c++ -*- */
/*
 * Copyright 2021 Analog Devices Inc.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef INCLUDED_OFDMRADAR_ARRAY_MUSIC_IMPL_H
#define INCLUDED_OFDMRADAR_ARRAY_MUSIC_IMPL_H

#include <ofdmradar/array_music.h>

namespace gr {
namespace ofdmradar {

class array_music_impl : public array_music
{
private:
    int d_array_size;
    int d_output_resolution;
    int d_targets;

public:
    array_music_impl(int array_size, int output_resolution, int targets);
    ~array_music_impl();

    int work(int noutput_items,
             gr_vector_const_void_star &input_items,
             gr_vector_void_star &output_items);
};

} // namespace ofdmradar
} // namespace gr

#endif /* INCLUDED_OFDMRADAR_ARRAY_MUSIC_IMPL_H */
