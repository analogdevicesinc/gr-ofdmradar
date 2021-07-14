/* -*- c++ -*- */
/*
 * Copyright 2021 Analog Devices, Inc.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef INCLUDED_OFDMRADAR_OFDMRADAR_RX_H
#define INCLUDED_OFDMRADAR_OFDMRADAR_RX_H

#include <gnuradio/block.h>
#include <ofdmradar/api.h>
#include <ofdmradar/ofdmradar.h>

namespace gr {
namespace ofdmradar {

/*!
 * \brief OFDM Radar Receiver. Output is the periodogram
 * \ingroup ofdmradar
 *
 */
class OFDMRADAR_API ofdmradar_rx : virtual public gr::block
{
public:
    typedef std::shared_ptr<ofdmradar_rx> sptr;

    /*!
     * \brief Return a shared_ptr to a new instance of ofdmradar::ofdmradar_rx.
     *
     * To avoid accidental use of raw pointers, ofdmradar::ofdmradar_rx's
     * constructor is in a private implementation
     * class. ofdmradar::ofdmradar_rx::make is the public interface for
     * creating new instances.
     */
    static sptr make(ofdmradar_params::sptr ofdm_params,
                     const std::string &len_tag_key,
                     size_t buffer_size);
};

} // namespace ofdmradar
} // namespace gr

#endif /* INCLUDED_OFDMRADAR_OFDMRADAR_RX_H */
