/* -*- c++ -*- */
/*
 * Copyright 2021 Analog Devices, Inc.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "ofdmradar_widget.h"

#include <QObject>
#include <QTimer>
#include <QWidget>
#include <Qt>

OFDMRadarWidget::OFDMRadarWidget(ofdmradar_params::sptr ofdm_params, QWidget *parent)
    : QWidget(parent), d_ofdm_params(ofdm_params)
{
    d_layout = new QVBoxLayout(this);

    OFDMRadarControls *ofdm_controls = new OFDMRadarControls(this);
    d_radar_controls = ofdm_controls;

    OFDMRadarScreen *ofdm_screen = new OFDMRadarScreen(ofdm_params, this);
    d_radar_screen = ofdm_screen;

    d_layout->addLayout(ofdm_controls);
    d_layout->addWidget(ofdm_screen);
    d_layout->setStretch(1, 10);

    connect(d_radar_controls,
            &OFDMRadarControls::settingsChanged,
            d_radar_screen,
            &OFDMRadarScreen::updateSettings);

    d_radar_controls->emitSettingsSignal();

    setLayout(d_layout);
}

OFDMRadarScreen *OFDMRadarWidget::getRadarScreen() { return d_radar_screen; }

OFDMRadarControls *OFDMRadarWidget::getRadarControls() { return d_radar_controls; }
