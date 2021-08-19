/* -*- c++ -*- */
/*
 * Copyright 2021 Analog Devices Inc.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "array_calib_impl.h"

#include <gnuradio/io_signature.h>
#include <pmt/pmt.h>

#include <Eigen/Dense>

#include <cmath>
#include <complex>

using namespace std::literals;

namespace gr {
namespace ofdmradar {

array_calib::sptr array_calib::make(int array_size, int targets, float pilot_angle)
{
    return gnuradio::make_block_sptr<array_calib_impl>(array_size, targets, pilot_angle);
}

namespace {
float omega_from_angle(float phi, float delta_x)
{
    return 2 * M_PIf32 * std::sin(-phi) * delta_x;
}
} // namespace


array_calib_impl::array_calib_impl(int array_size, int targets, float pilot_angle)
    : gr::sync_block(
          "array_calib",
          gr::io_signature::make(1, 1, array_size * array_size * sizeof(gr_complex)),
          gr::io_signature::make(0, 0, 0)),
      d_array_size(array_size),
      d_targets(targets),
      d_pilot_angle(pilot_angle),
      d_steering_vector(array_size),
      d_calib_msg_port_id(pmt::intern("calib")),
      d_trigger_msg_port_id(pmt::intern("trigger")),
      d_calib_requested(false),
      d_gamma(array_size, 1.0f)
{
    // Generate steering vector a(theta)
    const float omega = omega_from_angle(pilot_angle, 0.5);
    for (int i = 0; i < array_size; i++) {
        const gr_complex tmp = 2.0fi * M_PIf32 * omega;
        d_steering_vector[i] = std::exp(tmp);
    }

    message_port_register_in(d_trigger_msg_port_id);
    set_msg_handler(d_trigger_msg_port_id,
                             [this](pmt::pmt_t msg) { d_calib_requested.store(true); });

    message_port_register_out(d_calib_msg_port_id);
}

array_calib_impl::~array_calib_impl() {}

int array_calib_impl::work(int noutput_items,
                           gr_vector_const_void_star &input_items,
                           gr_vector_void_star &output_items)
{
    const gr_complex *in = reinterpret_cast<const gr_complex *>(input_items[0]);

    /*
     * Based on
     * V. C. Soon, L. Tong, Y. F. Huang and R. Liu,
     * "A Subspace Method for Estimating Sensor Gains and Phases,"
     * in IEEE Transactions on Signal Processing,
     * vol. 42, no. 4, pp. 973-976, Apr 1994.
     */

    using Eigen::ComplexEigenSolver;
    using Eigen::Map;
    using Eigen::MatrixXcf;
    using Eigen::VectorXcf;

    bool cmp = true;
    if (!d_calib_requested.compare_exchange_weak(cmp, false, std::memory_order_acquire))
        return noutput_items;

    Map<const MatrixXcf> imat(in, d_array_size, d_array_size);
    Map<VectorXcf> D_i(d_steering_vector.data(), d_array_size);

    const auto &&signal_space = imat.rightCols(d_targets);

    const auto W = D_i.adjoint().asDiagonal() * signal_space * signal_space.adjoint() *
                   D_i.asDiagonal();

    ComplexEigenSolver<MatrixXcf> eigensolver(W);

    VectorXcf Gamma(d_array_size);
    Gamma = eigensolver.eigenvectors().rightCols(1);

    Map<VectorXcf> old_Gamma(d_gamma.data(), d_array_size);
    Gamma.array() *= old_Gamma.array();

    old_Gamma = Gamma;

    pmt::pmt_t msg =
        pmt::make_blob(Gamma.array().data(), d_array_size * sizeof(gr_complex));

    message_port_pub(d_calib_msg_port_id, msg);

    return noutput_items;
}

} /* namespace ofdmradar */
} /* namespace gr */
