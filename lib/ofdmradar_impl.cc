/* -*- c++ -*- */
/*
 * Copyright 2021 Analog Devices, Inc.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "ofdmradar_impl.h"

#include <gnuradio/io_signature.h>
#include <ofdmradar/ofdmradar.h>

#include <boost/format.hpp>

#include <algorithm>
#include <cmath>
#include <complex>
#include <sstream>

namespace gr {
namespace ofdmradar {

namespace {

using namespace std::complex_literals;

void constellation_normalize_energy(std::vector<gr_complex> &constellation)
{
    float total = 0;

    for (auto s : constellation)
        total += s.real() * s.real() + s.imag() * s.imag();

    float fact = std::sqrt((float)constellation.size() / total);

    for (auto &s : constellation)
        s *= fact;
}

void constellation_center(std::vector<gr_complex> &constellation)
{
    gr_complex sum = 0;

    for (auto s : constellation)
        sum += s;

    gr_complex center = sum / (float)(constellation.size());
    for (auto &s : constellation)
        s -= center;
}

} // namespace

std::vector<gr_complex> get_constellation(modulation_scheme scheme, int order)
{
    std::vector<gr_complex> constellation(0);

    switch (scheme) {
    case modulation_scheme::BPSK:
        return { 1.f, -1.f }; // All symbols are on the unit circle

    case modulation_scheme::QPSK:
        constellation = { 0, 1, 1if, 1.f + 1if };
        break;

    case modulation_scheme::PSK:
        if (order < 2)
            throw std::runtime_error(boost::str(
                boost::format("%s: Invalid constellation order: %d") % __func__ % order));

        constellation.resize(order);

        for (int i = 0; i < order; i++)
            constellation[i] = std::cos(2 * (float)M_PIf32 * i / order) +
                               1if * std::sin(2 * (float)M_PIf32 * i / order);

        return constellation; // All symbols are on the unit circle

    case modulation_scheme::QAM: {
        if (order < 4 || __builtin_popcount(order) != 1 || !(__builtin_ffs(order) & 1))
            throw std::runtime_error(boost::str(
                boost::format("%s: Invalid QAM order requested: %d") % __func__ % order));

        constellation.resize(order);

        int edge = order >> ((__builtin_ffs(order) - 1) >> 1); // int 2**x sqrt
        for (int x = 0; x < edge; x++)
            for (int y = 0; y < edge; y++)
                constellation[x * edge + y] = (gr_complex)x + 1if * (float)y;

        break;
    }
    default:
        throw std::runtime_error(
            boost::str(boost::format("%s: Requested unsupported scheme!") % __func__));
    }

    constellation_center(constellation);
    constellation_normalize_energy(constellation);
    return constellation;
}

ofdmradar_params::sptr ofdmradar_params::make(unsigned int carriers,
                                              unsigned int symbols,
                                              unsigned int cyclic_prefix_length,
                                              unsigned int dc_guard,
                                              unsigned int nyquist_guard,
                                              std::vector<gr_complex> constellation,
                                              unsigned int seed)
{
    return std::make_shared<ofdmradar_params>(carriers,
                                              symbols,
                                              cyclic_prefix_length,
                                              dc_guard,
                                              nyquist_guard,
                                              std::move(constellation),
                                              seed);
}

ofdmradar_params::ofdmradar_params(unsigned int carriers,
                                   unsigned int symbols,
                                   unsigned int cyclic_prefix_length,
                                   unsigned int dc_guard,
                                   unsigned int nyquist_guard,
                                   std::vector<gr_complex> constellation,
                                   unsigned int seed)
    : d_carriers(carriers),
      d_symbols(symbols),
      d_cyclic_prefix_length(cyclic_prefix_length),
      d_dc_guard(dc_guard),
      d_nyquist_guard(nyquist_guard),
      d_constellation(std::move(constellation)),
      d_seed(seed),
      d_symbol_length(carriers + cyclic_prefix_length),
      d_frame_length(d_symbol_length * symbols),
      d_carrier_mask(carriers)
{
    // Generate carrier mask
    for (size_t i = 0; i < d_carrier_mask.size(); i++)
        d_carrier_mask[i] = !(i >= carriers - dc_guard || i <= dc_guard) &&
                            (i + nyquist_guard - (carriers / 2) >= nyquist_guard * 2);
}

namespace {

std::string constellation_to_string(const std::vector<gr_complex> cst)
{
    if (cst.size() == 0)
        return "[]";

    std::stringstream ss;

    ss << "[";

    for (int i = 0; i < cst.size() - 1; i++)
        ss << cst[i].real() << "+" << cst[i].imag() << "j, ";

    auto last = cst.back();
    ss << last.real() << "+" << last.imag() << "j]";

    return ss.str();
}

} // namespace

std::string ofdmradar_params::python_str()
{
    return boost::str(
        boost::format(
            "{carriers: %u, symbols: %u, cyclic_prefix_length: %u, dc_guard: %u, "
            "nyquist_guard: %u, constellation: %s, seed: %u}") %
        carriers() % symbols() % cyclic_prefix_length() % dc_guard() % nyquist_guard() %
        constellation_to_string(constellation()) % seed());
}

std::string ofdmradar_params::python_repr()
{
    return boost::str(boost::format("ofdmradar_params(carriers=%u, symbols=%u, "
                                    "cyclic_prefix_length=%u, dc_guard=%u, "
                                    "nyquist_guard=%u, constellation=%s, seed=%u)") %
                      carriers() % symbols() % cyclic_prefix_length() % dc_guard() %
                      nyquist_guard() % constellation_to_string(constellation()) %
                      seed());
}

//
// ofdmradar_shared: Shared stuff between RX and TX
//

ofdmradar_shared::ofdmradar_shared(ofdmradar_params::sptr ofdm_params)
    : d_ofdm_params(ofdm_params),
      d_generator(ofdm_params->seed()),
      d_distribution(0, ofdm_params->constellation().size() - 1)
{
    auto fft_size =
        std::max(ofdm_params->carriers(), ofdm_params->symbols()) * sizeof(fftwf_complex);
    d_fft_in = (fftwf_complex *)fftwf_malloc(fft_size);
    if (!d_fft_in)
        throw std::runtime_error("ofdmradar_tx: Failed to allocate FFT in buffer!");

    d_fft_out = (fftwf_complex *)fftwf_malloc(fft_size);
    if (!d_fft_out) {
        fftwf_free(d_fft_in);
        throw std::runtime_error("ofdmradar_tx: Failed to allocate FFT out buffer!");
    }

    d_fft_plan = fftwf_plan_dft_1d(
        d_ofdm_params->carriers(), d_fft_in, d_fft_out, FFTW_FORWARD, FFTW_MEASURE);
    if (!d_fft_plan) {
        fftwf_free(d_fft_in);
        fftwf_free(d_fft_out);
        throw std::runtime_error("ofdmradar_tx: Failed to create execution plan!");
    }

    d_ifft_plan = fftwf_plan_dft_1d(
        d_ofdm_params->carriers(), d_fft_in, d_fft_out, FFTW_BACKWARD, FFTW_MEASURE);
    if (!d_ifft_plan) {
        fftwf_destroy_plan(d_fft_plan);
        fftwf_free(d_fft_in);
        fftwf_free(d_fft_out);
        throw std::runtime_error("ofdmradar_tx: Failed to create ifft execution plan!");
    }

    d_fft_gr_in = reinterpret_cast<gr_complex *>(d_fft_in);
    d_fft_gr_out = reinterpret_cast<gr_complex *>(d_fft_out);
}

ofdmradar_shared::~ofdmradar_shared()
{
    fftwf_destroy_plan(d_ifft_plan);
    fftwf_destroy_plan(d_fft_plan);
    fftwf_free(d_fft_in);
    fftwf_free(d_fft_out);
}

void ofdmradar_shared::generate_tx_symbols(gr_complex *out)
{
    for (unsigned int i = 0; i < d_ofdm_params->carriers(); i++) {
        if (d_ofdm_params->carrier_mask()[i])
            out[i] = d_ofdm_params->constellation()[d_distribution(d_generator)];
        else
            out[i] = 0;
    }
}

} /* namespace ofdmradar */
} /* namespace gr */
