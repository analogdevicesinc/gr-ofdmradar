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
      d_targets(targets)
{
}

array_music_impl::~array_music_impl() {}

int array_music_impl::work(int noutput_items,
                           gr_vector_const_void_star &input_items,
                           gr_vector_void_star &output_items)
{
    const gr_complex *in = reinterpret_cast<const gr_complex *>(input_items[0]);
    float *out = reinterpret_cast<float *>(output_items[0]);

    std::cout << "music: noutput_items: " << noutput_items << std::endl;

    using Eigen::ArrayXf;
    using Eigen::Dynamic;
    using Eigen::Map;
    using Eigen::Matrix;
    using Eigen::MatrixXcf;
    using Eigen::VectorXcf;
    using Eigen::VectorXf;

    Map<const MatrixXcf> imat(in, d_array_size, d_array_size);

    VectorXcf eigvals(d_array_size);
    MatrixXcf eigvecs(d_array_size, d_array_size);
    Eigen::SelfAdjointEigenSolver<MatrixXcf> eigensolver(imat);

    eigvals = eigensolver.eigenvalues();
    eigvecs = eigensolver.eigenvectors();

    ArrayXf absolutes(d_array_size);
    absolutes = eigvals.array().abs();

    struct tuple {
        float v;
        int idx;
    };

    std::vector<tuple> sorted_vecs(d_array_size);
    for (int i = 0; i < d_array_size; i++) {
        sorted_vecs[i].v = absolutes(i);
        sorted_vecs[i].idx = i;
    }

    std::sort(sorted_vecs.begin(), sorted_vecs.end(), [](tuple &a, tuple &b) {
        return a.v > b.v;
    });

    for (int i = 0; i < d_array_size; i++)
        std::cout << "v: " << sorted_vecs[i].v << ", idx: " << sorted_vecs[i].idx
                  << std::endl;

    Map<VectorXf> ovec(out, d_output_resolution);
    consume_each(1);
    std::cout << "Done!" << std::endl;
    return 1;
}

} /* namespace ofdmradar */
} /* namespace gr */
