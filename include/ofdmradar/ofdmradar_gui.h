/* -*- c++ -*- */
/*
 * Copyright 2021 Analog Devices, Inc.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef INCLUDED_OFDMRADAR_OFDMRADAR_GUI_H
#define INCLUDED_OFDMRADAR_OFDMRADAR_GUI_H

#include <ofdmradar/api.h>
#include <ofdmradar/ofdmradar.h>

#include <gnuradio/sync_block.h>

#include <cstdint>
#include <memory>

class QWidget;

namespace gr {
namespace ofdmradar {

/*!
 * \brief The OFDM Radar GUI
 */
class OFDMRADAR_API ofdmradar_gui : virtual public gr::sync_block
{
public:
    typedef std::shared_ptr<ofdmradar_gui> sptr;

    static sptr make(ofdmradar_params::sptr ofdm_params, QWidget *parent = nullptr);

    virtual ~ofdmradar_gui() = default;

    virtual uintptr_t pyqwidget() = 0;
};

} // namespace ofdmradar
} // namespace gr

#endif /* INCLUDED_OFDMRADAR_OFDMRADAR_GUI_H */
