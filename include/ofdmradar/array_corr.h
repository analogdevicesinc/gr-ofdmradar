/* -*- c++ -*- */
/*
 * Copyright 2021 Analog Devices Inc.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef INCLUDED_OFDMRADAR_ARRAY_CORR_H
#define INCLUDED_OFDMRADAR_ARRAY_CORR_H

#include <gnuradio/block.h>
#include <ofdmradar/api.h>

namespace gr {
namespace ofdmradar {

/*!
 * \brief <+description of block+>
 * \ingroup ofdmradar
 *
 */
class OFDMRADAR_API array_corr : virtual public gr::block
{
public:
    typedef std::shared_ptr<array_corr> sptr;

    /*!
     * \brief Return a shared_ptr to a new instance of ofdmradar::array_corr.
     *
     * To avoid accidental use of raw pointers, ofdmradar::array_corr's
     * constructor is in a private implementation
     * class. ofdmradar::array_corr::make is the public interface for
     * creating new instances.
     */
    static sptr make(int array_size, int samples);
};

} // namespace ofdmradar
} // namespace gr

#endif /* INCLUDED_OFDMRADAR_ARRAY_CORR_H */
