/* -*- c++ -*- */
/*
 * Copyright 2021 Analog Devices, Inc.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "ofdmradar_screen.h"

#include <QList>
#include <QSizePolicy>

#include <iostream>

OFDMRadarScreen::OFDMRadarScreen(ofdmradar_params::sptr ofdm_params, QWidget *parent)
    : QOpenGLWidget(parent),
      d_ofdm_params(ofdm_params),
      d_front_buffer(ofdm_params->carriers() * ofdm_params->symbols()),
      d_back_buffer(ofdm_params->carriers() * ofdm_params->symbols())
{
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    setAutoFillBackground(false);
}

QSize OFDMRadarScreen::sizeHint() const { return QSize(1, 1); }

namespace {
void glerror(const char *where, int line)
{
    GLenum err;
    while ((err = glGetError()) != GL_NO_ERROR) {
        std::cout << "Ahh, error at " << where << ":" << line << ": " << err << std::endl;
    }
}
} // namespace

#define GLE glerror(__FILE__, __LINE__)

void OFDMRadarScreen::initializeGL()
{
    // connect(context(), &QOpenGLContext::aboutToBeDestroyed, this,
    // &RenderWidget::cleanup);

    initializeOpenGLFunctions();

    glClearColor(0, 0, 0, 1);

    d_program = new QOpenGLShaderProgram();
    bool ret = d_program->addShaderFromSourceFile(QOpenGLShader::Vertex, ":/screen.vert");
    if (!ret)
        throw std::runtime_error("Failed to compile vertex shader!");

    ret = d_program->addShaderFromSourceFile(QOpenGLShader::Fragment, ":/screen.frag");
    if (!ret)
        throw std::runtime_error("Failed to compile fragment shader!");

    if (!d_program->link())
        throw std::runtime_error("Failed to link shader programs!");

    /*
    glGenTextures(1, &d_texture_handle); GLE;

    glActiveTexture(GL_TEXTURE0); GLE;
    glBindTexture(GL_TEXTURE_RECTANGLE, d_texture_handle); GLE;
    // glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_WRAP_S, GL_REPEAT); GLE;
    // glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_WRAP_T, GL_REPEAT); GLE;
    glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MIN_FILTER, GL_NEAREST); GLE;
    glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MAG_FILTER, GL_NEAREST); GLE;
    glTexImage2D(GL_TEXTURE_RECTANGLE,
                 0,
                 GL_RG,
                 d_ofdm_params->carriers(),
                 d_ofdm_params->symbols(),
                 0,
                 GL_RG,
                 GL_FLOAT,
                 d_front_buffer.data()); GLE;

    // glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_BASE_LEVEL, 0); GLE;
    // glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MAX_LEVEL, 0); GLE;
    */

    d_texture = new QOpenGLTexture(QOpenGLTexture::Target2D);
    d_texture->setFormat(QOpenGLTexture::RG32F);
    d_texture->setSize(d_ofdm_params->carriers(), d_ofdm_params->symbols());
    d_texture->allocateStorage(QOpenGLTexture::RG, QOpenGLTexture::Float32);
    d_texture->setData(0, 0, 0, d_ofdm_params->carriers(), d_ofdm_params->symbols(),
            1, QOpenGLTexture::RG, QOpenGLTexture::Float32, d_front_buffer.data());
    d_texture->setMinificationFilter(QOpenGLTexture::LinearMipMapNearest);
    d_texture->setMagnificationFilter(QOpenGLTexture::Nearest);

    d_program->bind();
    d_program->setUniformValue("width", 400);
    d_program->setUniformValue("height", 400);
    d_program->setUniformValue("minV", d_min);
    d_program->setUniformValue("maxV", d_max);
    d_program->setUniformValue("rangeV", d_range);
    d_program->setUniformValue("screenData", 0); // Texture unit 0
    d_program->release();
}

void OFDMRadarScreen::resizeGL(int w, int h)
{
    d_program->bind();
    d_program->setUniformValue("width", w);
    d_program->setUniformValue("height", h);
    d_program->release();
}

void OFDMRadarScreen::paintGL()
{
    bool updated = false;
    {
        const std::lock_guard<std::mutex> lock(d_buffer_mutex);
        if (d_new_data) {
            std::swap(d_front_buffer, d_back_buffer);
            d_new_data = false;
            updated = true;
        }
    }

    if (updated) {
        d_texture->setData(0, 0, 0, d_ofdm_params->carriers(), d_ofdm_params->symbols(),
                1, QOpenGLTexture::RG, QOpenGLTexture::Float32, d_front_buffer.data());
    }

    glClear(GL_COLOR_BUFFER_BIT);

    // glBindTexture(GL_TEXTURE_RECTANGLE, d_texture_handle);
    d_texture->bind();

    d_program->bind();
    d_program->setUniformValue("minV", d_min);
    d_program->setUniformValue("maxV", d_max);
    d_program->setUniformValue("rangeV", d_range);

    glRectf(-1.0f, -1.0f, 1.0f, 1.0f);

    glFlush();
    d_program->release();
}

void OFDMRadarScreen::updateSettings(float min, float max, float range)
{
    d_min = min;
    if (d_max < d_min)
        d_max = d_min;
    else
        d_max = max;

    d_range = range;

    update();
}

void OFDMRadarScreen::updateTexture() {}

void OFDMRadarScreen::submitBuffer(const std::complex<float> *data, size_t len)
{
    {
        const std::lock_guard<std::mutex> lock(d_buffer_mutex);
        if (len != d_back_buffer.size())
            throw std::runtime_error(
                "ofdmradar_screen::submitBuffer: Errr, length missmatch in?!");

        std::memcpy(d_back_buffer.data(), data, sizeof(d_back_buffer[0]) * len);
        d_new_data = true;
    }
    this->update();
}
