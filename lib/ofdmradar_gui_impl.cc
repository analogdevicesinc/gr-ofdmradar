/* -*- c++ -*- */
/*
 * Copyright 2021 Analog Devices, Inc.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "ofdmradar_gui_impl.h"

#include <gnuradio/io_signature.h>
#include <gnuradio/prefs.h>
#include <gnuradio/qtgui/utils.h>

#include <QObject>
#include <QSurfaceFormat>

namespace gr {
namespace ofdmradar {

ofdmradar_gui::sptr ofdmradar_gui::make(ofdmradar_params::sptr params, QWidget *parent)
{
    return std::make_shared<ofdmradar_gui_impl>(params, parent);
}

ofdmradar_gui_impl::ofdmradar_gui_impl(ofdmradar_params::sptr ofdm_params,
                                       QWidget *parent)
    : gr::sync_block("ofdmradar_gui",
                     gr::io_signature::make(1, 1, sizeof(gr_complex)),
                     gr::io_signature::make(0, 0, 0)),
      d_parent(parent),
      d_ofdm_params(ofdm_params),
      d_buffer_size(ofdm_params->peri_length())
{
    this->set_output_multiple(d_buffer_size);
    initialize_qt();
}

void ofdmradar_gui_impl::initialize_qt()
{
    /*
    QSurfaceFormat format;
    format.setMajorVersion(4);
    format.setMinorVersion(2);
    format.setProfile(QSurfaceFormat::CoreProfile);
    format.setOption(QSurfaceFormat::DebugContext);
    QSurfaceFormat::setDefaultFormat(format);
    */

    if (qApp != NULL) {
        d_qApplication = qApp;
    } else {
        // Yes, this is technically a leak, but its size is insiginificant, and the
        // QT Application requires writable storage with a lifetime >= that of the
        // qApplication.
        char *arg = new char;
        char **argv = new char *;
        *arg = '\0';
        *argv = arg;
        int *argc = new int;
        *argc = 1;
        d_qApplication = new QApplication(*argc, argv);
    }

    check_set_qss(d_qApplication);

    d_qwidget = new OFDMRadarWidget(d_ofdm_params, d_parent);
}

uintptr_t ofdmradar_gui_impl::pyqwidget() { return (uintptr_t)d_qwidget; }

ofdmradar_gui_impl::~ofdmradar_gui_impl() {}

int ofdmradar_gui_impl::work(int noutput_items,
                             gr_vector_const_void_star &input_items,
                             gr_vector_void_star &output_items) {

    const gr_complex *in = reinterpret_cast<const gr_complex *>(input_items[0]);

    d_qwidget->getRadarScreen()->submitBuffer(in, d_buffer_size);
    return d_buffer_size;
}

} /* namespace ofdmradar */
} /* namespace gr */
