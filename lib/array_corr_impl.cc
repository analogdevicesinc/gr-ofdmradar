/* -*- c++ -*- */
/*
 * Copyright 2021 Analog Devices Inc.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "array_corr_impl.h"

#include <gnuradio/io_signature.h>

#include <pmt/pmt.h>

#include <Eigen/Dense>

#include <boost/format.hpp>

#include <string>

namespace gr {
namespace ofdmradar {

array_corr::sptr array_corr::make(int array_size, int samples)
{
    return gnuradio::make_block_sptr<array_corr_impl>(array_size, samples);
}

/*
 * The private constructor
 */
array_corr_impl::array_corr_impl(int array_size, int samples)
    : gr::block(
          "array_corr",
          gr::io_signature::make(1, 1, array_size * sizeof(gr_complex)),
          gr::io_signature::make(1, 1, array_size * array_size * sizeof(gr_complex))),
      d_samples(samples),
      d_array_size(array_size),
      d_calib_buffer(array_size * array_size, 1.0f)
{
    message_port_register_in(pmt::intern("calib"));

    set_msg_handler(pmt::intern("calib"),
                    [this](pmt::pmt_t msg) { this->handle_calib_data(msg); });
}

void array_corr_impl::handle_calib_data(pmt::pmt_t msg)
{
    if (!pmt::is_blob(msg)) {
        GR_LOG_WARN(d_logger, "Received invalid message on calib message port!");
        return;
    }

    const size_t expected = sizeof(gr_complex) * d_array_size;
    if (pmt::blob_length(msg) != expected) {
        GR_LOG_WARN(d_logger,
                    boost::format("Calibration blob: Expected %lu bytes, got %lu.") %
                        expected % pmt::blob_length(msg));
        return;
    }

    const gr_complex *data = reinterpret_cast<const gr_complex *>(pmt::blob_data(msg));
    using Eigen::Map;
    using Eigen::MatrixXcf;
    using Eigen::VectorXcf;

    Map<const VectorXcf> Gamma(data, d_array_size);
    Map<MatrixXcf> cal(d_calib_buffer.data(), d_array_size, d_array_size);
    VectorXcf tmp(d_array_size);
    tmp = 0.5f / Gamma.array();

    cal = tmp * tmp.adjoint();
}

void array_corr_impl::forecast(int noutput_items, gr_vector_int &ninput_items_required)
{
    int req = static_cast<int>(std::ceil(noutput_items * d_samples));

    ninput_items_required[0] = req;
}

int array_corr_impl::general_work(int noutput_items,
                                  gr_vector_int &ninput_items,
                                  gr_vector_const_void_star &input_items,
                                  gr_vector_void_star &output_items)
{
    const gr_complex *in = reinterpret_cast<const gr_complex *>(input_items[0]);
    gr_complex *out = reinterpret_cast<gr_complex *>(output_items[0]);

    using Eigen::Map;
    using Eigen::MatrixXcf;

    if (ninput_items[0] < d_samples)
        return 0;

    // Yes i know, no synchronization. But what's the worst that can happen? A couple
    // weird outputs just when you perform the calibration?
    Map<const MatrixXcf> cal(d_calib_buffer.data(), d_array_size, d_array_size);

    int ret;
    for (ret = 0; (ret + 1) * d_samples <= ninput_items[0] && ret < noutput_items;
         ret++) {
        Map<const MatrixXcf> inmat(in, d_array_size, d_samples);

        MatrixXcf R_xx(d_array_size, d_array_size);
        R_xx = inmat * inmat.conjugate().transpose() / d_samples;

        R_xx.array() *= cal.array();

        MatrixXcf eigvecs(d_array_size, d_array_size);
        Eigen::SelfAdjointEigenSolver<MatrixXcf> eigensolver(R_xx);

        Map<MatrixXcf> outmat(out, d_array_size, d_array_size);
        outmat = eigensolver.eigenvectors();

        in += d_samples * d_array_size;
        out += d_array_size * d_array_size;
    }

    consume_each(ret * d_samples);
    return ret;
}

/*
 * Our virtual destructor.
 */
array_corr_impl::~array_corr_impl() {}

} /* namespace ofdmradar */
} /* namespace gr */
