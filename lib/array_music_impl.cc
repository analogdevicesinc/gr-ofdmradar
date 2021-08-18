/* -*- c++ -*- */
/*
 * Copyright 2021 Analog Devices Inc.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "array_music_impl.h"

#include <gnuradio/io_signature.h>

#include <Eigen/Dense>

#include <algorithm>
#include <cmath>
#include <vector>

using namespace std::complex_literals;

namespace gr {
namespace ofdmradar {

array_music::sptr array_music::make(int array_size, int output_resolution, int targets)
{
    return gnuradio::make_block_sptr<array_music_impl>(
        array_size, output_resolution, targets);
}

array_music_impl::array_music_impl(int array_size, int output_resolution, int targets)
    : gr::sync_block(
          "array_music",
          gr::io_signature::make(1, 1, array_size * array_size * sizeof(gr_complex)),
          gr::io_signature::make(1, 1, output_resolution * sizeof(float))),
      d_array_size(array_size),
      d_output_resolution(output_resolution),
      d_targets(targets),
      d_steering_vectors(array_size, output_resolution)
{
    const float half = output_resolution / 2.0f;

    for (int u = 0; u < output_resolution; u++) {
        float phi = (u - half) / output_resolution * M_PIf32;
        float omega = std::sin(phi) * M_PIf32;
        for (int v = 0; v < array_size; v++)
            d_steering_vectors(v, u) =
                std::exp(static_cast<gr_complex>(1.0fi * omega * v));
    }
}

array_music_impl::~array_music_impl() {}

int array_music_impl::work(int noutput_items,
                           gr_vector_const_void_star &input_items,
                           gr_vector_void_star &output_items)
{
    const gr_complex *in = reinterpret_cast<const gr_complex *>(input_items[0]);
    float *out = reinterpret_cast<float *>(output_items[0]);

    using Eigen::ArrayXf;
    using Eigen::Dynamic;
    using Eigen::Map;
    using Eigen::Matrix;
    using Eigen::MatrixXcf;
    using Eigen::VectorXcf;
    using Eigen::VectorXf;

    for (int s = 0; s < noutput_items; s++) {
        Map<const MatrixXcf> imat(in, d_array_size, d_array_size);

        const int noise_dim = d_array_size - d_targets;
        auto&& noise_space = imat.leftCols(noise_dim);

        Map<VectorXf> ovec(out, d_output_resolution);

        MatrixXcf noise_tmp(d_array_size, d_array_size);
        noise_tmp = noise_space * noise_space.transpose().conjugate();

        for (int i = 0; i < d_output_resolution; i++) {
            const auto v = (d_steering_vectors.col(i).transpose().conjugate() *
                            noise_tmp * d_steering_vectors.col(i))(0, 0);
            ovec(i) = 1 / std::abs(v);
        }

        auto max = ovec.maxCoeff();
        if (max != 0)
            ovec /= max;

        in += d_array_size * d_array_size;
        out += d_output_resolution;
    }

    return noutput_items;
}

} /* namespace ofdmradar */
} /* namespace gr */
