/* -*- c++ -*- */
/*
 * Copyright 2021 Analog Devices Inc.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "array_corr_impl.h"

#include <gnuradio/io_signature.h>

#include <Eigen/Dense>

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
      d_array_size(array_size)
{
}

void array_corr_impl::forecast(int noutput_items, gr_vector_int &ninput_items_required)
{
    int req = static_cast<int>(std::ceil(noutput_items * d_samples));

    // std::fill(ninput_items_required.begin(), ninput_items_required.end(), req);
    ninput_items_required[0] = req;
}

int array_corr_impl::general_work(int noutput_items,
                                  gr_vector_int &ninput_items,
                                  gr_vector_const_void_star &input_items,
                                  gr_vector_void_star &output_items)
{
    const gr_complex *in = reinterpret_cast<const gr_complex *>(input_items[0]);
    gr_complex *out = reinterpret_cast<gr_complex *>(output_items[0]);

    using Eigen::MatrixXcf;
    using Eigen::Map;

    if (ninput_items[0] < d_samples)
        return 0;

    std::cout << "corr: d_samples: " << d_samples << ", ninput_items[0]: " << ninput_items[0] << std::endl;

    int ret;
    for (ret = 0; (ret + 1) * d_samples <= ninput_items[0] && ret < noutput_items; ret++) {
        Map<const MatrixXcf> inmat(in, d_array_size, d_samples);

        Map<MatrixXcf> outmat(out, d_array_size, d_array_size);

        outmat = inmat * inmat.conjugate().transpose() / d_samples;
        std::cout << "outmat:" << std::endl;
        std::cout << outmat << std::endl;

        in += d_samples * d_array_size;
        out += d_array_size * d_array_size;
    }

    consume_each(ret * d_samples);
    std::cout << "ret: " << ret << std::endl;
    // Tell runtime system how many output items we produced.
    return ret;
}

/*
 * Our virtual destructor.
 */
array_corr_impl::~array_corr_impl() {}

} /* namespace ofdmradar */
} /* namespace gr */
