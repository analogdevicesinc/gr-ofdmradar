/* -*- c++ -*- */
/*
 * Copyright 2021 Analog Devices, Inc.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef INCLUDED_OFDMRADAR_OFDMRADAR_GUI_IMPL_H
#define INCLUDED_OFDMRADAR_OFDMRADAR_GUI_IMPL_H

#include "gui/ofdmradar_widget.h"

#include <ofdmradar/ofdmradar_gui.h>

#include <qapplication.h>
#include <QTimer>

namespace gr {
namespace ofdmradar {

class ofdmradar_gui_impl : public ofdmradar_gui
{
    friend ofdmradar_gui;

    QApplication *d_qApplication;
    OFDMRadarWidget *d_qwidget;
    QWidget *d_parent;

    ofdmradar_params::sptr d_ofdm_params;
    size_t d_buffer_size;

    void initialize_qt();

public:
    typedef std::shared_ptr<ofdmradar_gui> sptr;

    ofdmradar_gui_impl(ofdmradar_params::sptr params, QWidget *parent = nullptr);
    ~ofdmradar_gui_impl();

    int work(int noutput_items,
             gr_vector_const_void_star &input_items,
             gr_vector_void_star &output_items);

    uintptr_t pyqwidget();
};

} // namespace ofdmradar
} // namespace gr

#endif /* INCLUDED_OFDMRADAR_OFDMRADAR_GUI_IMPL_H */
