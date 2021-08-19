/* -*- c++ -*- */
/*
 * Copyright 2021 Analog Devices Inc.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "array_esprit_impl.h"

#include <gnuradio/io_signature.h>

#include <Eigen/Dense>

#include <complex>
#include <cmath>

namespace gr {
namespace ofdmradar {

array_esprit::sptr array_esprit::make(int array_size, int targets)
{
    return gnuradio::make_block_sptr<array_esprit_impl>(array_size, targets);
}

array_esprit_impl::array_esprit_impl(int array_size, int targets)
    : gr::sync_block(
          "array_esprit",
          gr::io_signature::make(1, 1, array_size * array_size * sizeof(gr_complex)),
          gr::io_signature::make(1, 1, sizeof(float))),
      d_array_size(array_size),
      d_targets(targets)
{
}

array_esprit_impl::~array_esprit_impl() {}

namespace {

float angle_from_omega(float omega, float delta_x) {
    return std::asin(-omega / (2*M_PIf32 * delta_x));
}

}

int array_esprit_impl::work(int noutput_items,
                            gr_vector_const_void_star &input_items,
                            gr_vector_void_star &output_items)
{
    const gr_complex *in = reinterpret_cast<const gr_complex *>(input_items[0]);
    float *out = reinterpret_cast<float *>(output_items[0]);

    using Eigen::Map;
    using Eigen::MatrixXcf;
    using Eigen::ComplexEigenSolver;

    for (int i = 0; i < noutput_items; i++) {
        Map<const MatrixXcf> imat(in, d_array_size, d_array_size); // eigvecs(R_x)

        auto &&U_s = imat.rightCols(d_targets); // Signal space

        MatrixXcf tmp = MatrixXcf::Constant(d_array_size - 1, d_array_size, 0);

        tmp.block(0, 0, d_array_size - 1, d_array_size - 1) =
            MatrixXcf::Identity(d_array_size - 1, d_array_size - 1);

        MatrixXcf S_1 = tmp * U_s;

        tmp(0, 0) = 0;
        tmp.block(0, 1, d_array_size - 1, d_array_size - 1) =
            MatrixXcf::Identity(d_array_size - 1, d_array_size - 1);

        MatrixXcf S_2 = tmp * U_s;

        MatrixXcf Phi = (S_2.adjoint() * S_2).ldlt().solve(S_2.adjoint() * S_1);

        ComplexEigenSolver<MatrixXcf> Phi_eigensolver(Phi, false);
        auto eigenvals = Phi_eigensolver.eigenvalues();

        for (int i = 0; i < d_targets; i++)
            out[i] = angle_from_omega(std::arg(eigenvals(0)), 0.5f);

        in += d_array_size * d_array_size;
        out += d_targets;
    }


    return noutput_items;
}

} /* namespace ofdmradar */
} /* namespace gr */
