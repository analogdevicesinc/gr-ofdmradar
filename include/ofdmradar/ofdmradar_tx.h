/* -*- c++ -*- */
/*
 * Copyright 2021 Analog Devices, Inc.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef INCLUDED_OFDMRADAR_OFDMRADAR_TX_H
#define INCLUDED_OFDMRADAR_OFDMRADAR_TX_H

#include <gnuradio/sync_block.h>
#include <ofdmradar/api.h>
#include <ofdmradar/ofdmradar.h>

namespace gr {
namespace ofdmradar {

/*!
 * \brief The OFDM radar transmitter.
 * \ingroup ofdmradar
 *
 *  This block is responsible for the generation of the OFDM frame
 *
 */
class OFDMRADAR_API ofdmradar_tx : virtual public gr::sync_block
{
public:
    typedef std::shared_ptr<ofdmradar_tx> sptr;

    /*!
     * \brief Return a shared_ptr to a new instance of ofdmradar::ofdmradar_tx.
     *
     * To avoid accidental use of raw pointers, ofdmradar::ofdmradar_tx's
     * constructor is in a private implementation
     * class. ofdmradar::ofdmradar_tx::make is the public interface for
     * creating new instances.
     *
     * \param ofdm_params OFDM radar system parameters
     * \param len_tag_key Output data will be length-tagged with this key.
     */
    static sptr make(ofdmradar_params::sptr ofdm_params, const std::string &len_tag_key);
};

} // namespace ofdmradar
} // namespace gr

#endif /* INCLUDED_OFDMRADAR_OFDMRADAR_TX_H */
