/* -*- c++ -*- */
/*
 * Copyright 2021 Analog Devices, Inc.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef H_OFDMRADAR_GUI_OFDMRADAR_SCREEN
#define H_OFDMRADAR_GUI_OFDMRADAR_SCREEN

#include <ofdmradar/ofdmradar.h>

#include <QOpenGLFunctions>
#include <QOpenGLWidget>

#include <QBrush>
#include <QFont>
#include <QOpenGLDebugLogger>
#include <QOpenGLShaderProgram>
#include <QOpenGLTexture>
#include <QPen>
#include <QSize>
#include <QWidget>

#include <complex>
#include <functional>
#include <mutex>
#include <vector>

using gr::ofdmradar::ofdmradar_params;

class OFDMRadarScreen : public QOpenGLWidget, QOpenGLFunctions
{
    Q_OBJECT

private:
    ofdmradar_params::sptr d_ofdm_params;
    std::mutex d_buffer_mutex;

    bool d_new_data = false;

    std::vector<std::complex<float>> d_front_buffer;
    std::vector<std::complex<float>> d_back_buffer;

    QOpenGLShaderProgram *d_program;
    QOpenGLTexture *d_texture;
    GLuint d_texture_handle;

    float d_min = 0;
    float d_max = 1;
    float d_range = 1;
    float d_doppler_range = 1;

    void cleanupGL();

    void updateTexture();

public:
    OFDMRadarScreen(ofdmradar_params::sptr params, QWidget *parent = nullptr);

    void submitBuffer(const std::complex<float> *data, size_t length);

    QSize sizeHint() const override;

public slots:
    void updateSettings(float min, float max, float range, float doppler_range);

protected:
    void resizeGL(int w, int h) override;

    void paintGL() override;

    void initializeGL() override;
};

#endif // H_OFDMRADAR_GUI_OFDMRADAR_SCREEN
