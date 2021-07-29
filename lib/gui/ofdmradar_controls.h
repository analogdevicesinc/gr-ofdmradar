/* -*- c++ -*- */
/*
 * Copyright 2021 Analog Devices, Inc.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef H_OFDMRADAR_GUI_OFDMRADAR_CONTROLS
#define H_OFDMRADAR_GUI_OFDMRADAR_CONTROLS

#include <QFormLayout>
#include <QObject>
#include <QSlider>
#include <QWidget>

class OFDMRadarControls : public QFormLayout
{
    Q_OBJECT

private:
    QWidget *d_range_label;
    QSlider *d_range_slider;
    QSlider *d_doppler_range_slider;
    QSlider *d_min_slider;
    QSlider *d_max_slider;

public:
    OFDMRadarControls(QWidget *parent = nullptr);
    void emitSettingsSignal();

private slots:
    void slidersChanged(int v);

signals:
    void settingsChanged(float min, float max, float range, float doppler_range);
};

#endif // H_OFDMRADAR_GUI_OFDMRADAR_CONTROLS
