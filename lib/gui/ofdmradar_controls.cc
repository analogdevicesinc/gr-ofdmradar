/* -*- c++ -*- */
/*
 * Copyright 2021 Analog Devices, Inc.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "ofdmradar_controls.h"

#include <QLabel>
#include <QSlider>
#include <QSpacerItem>
#include <Qt>

#include <iostream>

OFDMRadarControls::OFDMRadarControls(QWidget *parent) : QFormLayout(parent)
{
    QLabel *range_label = new QLabel();
    range_label->setText(tr("Range: "));
    range_label->setAlignment(Qt::AlignLeft);
    d_range_label = range_label;

    QSlider *range_slider = new QSlider(Qt::Horizontal);
    range_slider->setMinimum(10);
    range_slider->setMaximum(1000);
    range_slider->setTickInterval(100);
    range_slider->setTickPosition(QSlider::TicksBelow);
    range_slider->setValue(1000);
    d_range_slider = range_slider;
    this->addRow(range_label, range_slider);
    connect(
        d_range_slider, &QSlider::valueChanged, this, &OFDMRadarControls::slidersChanged);

    QLabel *doppler_range_label = new QLabel();
    doppler_range_label->setText(tr("Doppler Range: "));
    doppler_range_label->setAlignment(Qt::AlignLeft);
    QSlider *doppler_range_slider = new QSlider(Qt::Horizontal);
    doppler_range_slider->setMinimum(10);
    doppler_range_slider->setMaximum(1000);
    doppler_range_slider->setTickInterval(100);
    doppler_range_slider->setTickPosition(QSlider::TicksBelow);
    doppler_range_slider->setValue(1000);
    d_doppler_range_slider = doppler_range_slider;
    this->addRow(doppler_range_label, doppler_range_slider);
    connect(
        d_doppler_range_slider, &QSlider::valueChanged, this, &OFDMRadarControls::slidersChanged);

    QLabel *min_label = new QLabel();
    min_label->setText(tr("Min: "));
    min_label->setAlignment(Qt::AlignLeft);
    d_min_slider = new QSlider(Qt::Horizontal);
    d_min_slider->setMinimum(0);
    d_min_slider->setMaximum(100);
    d_min_slider->setTickInterval(10);
    d_min_slider->setTickPosition(QSlider::TicksBelow);
    this->addRow(min_label, d_min_slider);
    connect(
        d_min_slider, &QSlider::valueChanged, this, &OFDMRadarControls::slidersChanged);

    QLabel *max_label = new QLabel();
    max_label->setText(tr("Max: "));
    max_label->setAlignment(Qt::AlignLeft);
    d_max_slider = new QSlider(Qt::Horizontal);
    d_max_slider->setMinimum(10);
    d_max_slider->setMaximum(110);
    d_max_slider->setTickInterval(10);
    d_max_slider->setTickPosition(QSlider::TicksBelow);
    d_max_slider->setValue(30);
    this->addRow(max_label, d_max_slider);
    connect(
        d_max_slider, &QSlider::valueChanged, this, &OFDMRadarControls::slidersChanged);
}

void OFDMRadarControls::slidersChanged(int v)
{
    emitSettingsSignal();
}

void OFDMRadarControls::emitSettingsSignal()
{
    emit settingsChanged(d_min_slider->value() / 10.f,
                         d_max_slider->value() / 10.f,
                         d_range_slider->value() / 1000.f,
                         d_doppler_range_slider->value() / 1000.0f);
}
