/* -*- c++ -*- */
/*
 * Copyright 2021 Analog Devices Inc.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef INCLUDED_OFDMRADAR_ARRAY_CALIB_H
#define INCLUDED_OFDMRADAR_ARRAY_CALIB_H

#include <gnuradio/sync_block.h>

#include <ofdmradar/api.h>

namespace gr {
namespace ofdmradar {

/*!
 * \brief Calibrates the array by assuming a known pilot angle is being sent
 * \ingroup ofdmradar
 *
 */
class OFDMRADAR_API array_calib : virtual public gr::sync_block
{
public:
    typedef std::shared_ptr<array_calib> sptr;

    /*!
     * \brief Return a shared_ptr to a new instance of ofdmradar::array_calib.
     *
     * To avoid accidental use of raw pointers, ofdmradar::array_calib's
     * constructor is in a private implementation
     * class. ofdmradar::array_calib::make is the public interface for
     * creating new instances.
     *
     * \param array_size The amount of elements in the linear array. Determines the width
     *                   of the input vector.
     * \param targets    The size of our signal space or how many sources we want to
     *                   estimate.
     * \param pilot_angle The pilot angle, aka. doa of the reference signal
     */
    static sptr make(int array_size, int targets, float pilot_angle);
};

} // namespace ofdmradar
} // namespace gr

#endif /* INCLUDED_OFDMRADAR_ARRAY_CALIB_H */
