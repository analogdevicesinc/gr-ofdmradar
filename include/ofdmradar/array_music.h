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
 * \brief Calculate the MUSIC pseudo spectrum from a correlation matrix as produced by
 *        array_corr.
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
     *
     * \param array_size The amount of elements in the linear array. Determines the width
     *                   of the input vector.
     * \param output_resolution The resolution of the music pseudo spectrum.
     * \param targets    The size of our signal space or how many sources we want to
     *                   estimate.
     */
    static sptr make(int array_size, int output_resolution, int targets);
};

} // namespace ofdmradar
} // namespace gr

#endif /* INCLUDED_OFDMRADAR_ARRAY_MUSIC_H */
