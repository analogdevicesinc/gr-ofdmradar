/* -*- c++ -*- */
/*
 * Copyright 2021 Analog Devices, Inc.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef H_OFDMRADAR_GUI_OFDMRADAR_WIDGET
#define H_OFDMRADAR_GUI_OFDMRADAR_WIDGET

#include "ofdmradar_controls.h"
#include "ofdmradar_screen.h"

#include <ofdmradar/ofdmradar.h>

#include <QGridLayout>
#include <QObject>
#include <QVBoxLayout>
#include <QWidget>

using gr::ofdmradar::ofdmradar_params;

class OFDMRadarWidget : public QWidget
{
    Q_OBJECT

private:
    ofdmradar_params::sptr d_ofdm_params;
    QVBoxLayout *d_layout;
    OFDMRadarControls *d_radar_controls;
    OFDMRadarScreen *d_radar_screen;

public:
    OFDMRadarWidget(ofdmradar_params::sptr, QWidget *parent = nullptr);

    OFDMRadarScreen *getRadarScreen();

    OFDMRadarControls *getRadarControls();
};

#endif // H_OFDMRADAR_GUI_OFDMRADAR_WIDGET
