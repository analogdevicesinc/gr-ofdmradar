/* -*- c++ -*- */
/*
 * Copyright 2021 Analog Devices Inc.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef INCLUDED_OFDMRADAR_ARRAY_MUSIC_H
#define INCLUDED_OFDMRADAR_ARRAY_MUSIC_H

#include <gnuradio/sync_block.h>
#include <ofdmradar/api.h>

namespace gr {
namespace ofdmradar {

/*!
 * \brief <+description of block+>
 * \ingroup ofdmradar
 *
 */
class OFDMRADAR_API array_music : virtual public gr::sync_block
{
public:
    typedef std::shared_ptr<array_music> sptr;

    /*!
     * \brief Return a shared_ptr to a new instance of ofdmradar::array_music.
     *
     * To avoid accidental use of raw pointers, ofdmradar::array_music's
     * constructor is in a private implementation
     * class. ofdmradar::array_music::make is the public interface for
     * creating new instances.
     */
    static sptr make(int array_size, int output_resolution, int targets);
};

} // namespace ofdmradar
} // namespace gr

#endif /* INCLUDED_OFDMRADAR_ARRAY_MUSIC_H */
