/* -*- c++ -*- */
/*
 * Copyright 2021 Analog Devices Inc.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef INCLUDED_OFDMRADAR_ARRAY_ESPRIT_H
#define INCLUDED_OFDMRADAR_ARRAY_ESPRIT_H

#include <gnuradio/sync_block.h>
#include <ofdmradar/api.h>

namespace gr {
namespace ofdmradar {

/*!
 * \brief <+description of block+>
 * \ingroup ofdmradar
 *
 */
class OFDMRADAR_API array_esprit : virtual public gr::sync_block
{
public:
    typedef std::shared_ptr<array_esprit> sptr;

    /*!
     * \brief Return a shared_ptr to a new instance of ofdmradar::array_esprit.
     *
     * To avoid accidental use of raw pointers, ofdmradar::array_esprit's
     * constructor is in a private implementation
     * class. ofdmradar::array_esprit::make is the public interface for
     * creating new instances.
     */
    static sptr make();
};

} // namespace ofdmradar
} // namespace gr

#endif /* INCLUDED_OFDMRADAR_ARRAY_ESPRIT_H */
