/* -*- c++ -*- */
/*
 * Copyright 2021 Analog Devices, Inc.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef INCLUDED_OFDMRADAR_OFDMRADAR_IMPL_H
#define INCLUDED_OFDMRADAR_OFDMRADAR_IMPL_H

#include <ofdmradar/ofdmradar.h>

#include <fftw3.h>
#include <cstdlib>
#include <random>

namespace gr {
namespace ofdmradar {

/*!
 * \brief Shared internals of ofdmradar blocks
 */
class ofdmradar_shared
{
private:
    std::default_random_engine d_generator;
    std::uniform_int_distribution<unsigned int> d_distribution;

protected:
    ofdmradar_params::sptr d_ofdm_params;

    fftwf_plan d_fft_plan;
    fftwf_plan d_ifft_plan;
    fftwf_complex *d_fft_in;
    fftwf_complex *d_fft_out;
    gr_complex *d_fft_gr_in;
    gr_complex *d_fft_gr_out;

    /*!
     * Selects a bunch of random symbols and writes them to out.
     */
    void generate_tx_symbols(gr_complex *out);

public:
    ofdmradar_shared(ofdmradar_params::sptr params);
    ~ofdmradar_shared();
};

} // namespace ofdmradar
} // namespace gr

#endif /* INCLUDED_OFDMRADAR_OFDMRADAR_IMPL_H */
