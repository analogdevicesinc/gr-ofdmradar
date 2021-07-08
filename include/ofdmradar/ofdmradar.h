/* -*- c++ -*- */
/*
 * Copyright 2021 Analog Devices, Inc.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef INCLUDED_OFDMRADAR_OFDMRADAR_H
#define INCLUDED_OFDMRADAR_OFDMRADAR_H

#include <gnuradio/sync_block.h>
#include <ofdmradar/api.h>

namespace gr {
namespace ofdmradar {

/*!
 * \brief Common OFDM radar system parameters
 * \ingroup ofdmradar
 */
class OFDMRADAR_API ofdmradar_params
    : public std::enable_shared_from_this<ofdmradar_params>
{
public:
    typedef std::shared_ptr<ofdmradar_params> sptr;

private:
    unsigned int d_carriers;
    unsigned int d_symbols;
    unsigned int d_cyclic_prefix_length;
    unsigned int d_dc_guard;
    unsigned int d_nyquist_guard;
    std::vector<gr_complex> d_constellation;
    unsigned int d_seed;
    std::vector<bool> d_carrier_mask;
    unsigned int d_symbol_length;
    unsigned int d_frame_length;

public:
    /*!
     * Size of the main OFDM FFT. Note: Not all carriers will actually be in use
     */
    unsigned int carriers() { return d_carriers; }

    /*!
     * How many symbols are used per OFDM frame
     */
    unsigned int symbols() { return d_symbols; }

    /*!
     * Length of the cyclic prefix in samples. The cyclic prefix is the amount of
     * samples at the end of OFDM symbol that will be prepended at the front
     */
    unsigned int cyclic_prefix_length() { return d_cyclic_prefix_length; }

    /*!
     * How many symbols around the DC carrier are to be excluded. This value is counted
     * on top of the DC carrier, which is always excluded, and counted per direction.
     * With a dc_guard of 1, three carriers in the middle (or 2*dc_guard+1) will be left
     * unused.
     */
    unsigned int dc_guard() { return d_dc_guard; }

    /*!
     * The nyquist_guard determines the total bandwidth of the signal, by controlling
     * how many bins of the FFT will be left empty on each side.
     * The two-sided bandwidth can be calculated as B = f_s * (1-nyquist_guard/N*2),
     * where f_s is the complex sample rate, and N the FFT size (see "carriers").
     */
    unsigned int nyquist_guard() { return d_nyquist_guard; }

    /*!
     * A list of symbols comprising the constellation
     */
    const std::vector<gr_complex> &constellation() { return d_constellation; }

    /*!
     * RNG seed used to select symbols out of constellation.
     */
    unsigned int seed() { return d_seed; }

    /*!
     * Total symbol length in samples
     */
    unsigned int symbol_length() { return d_symbol_length; }

    /*!
     * Total frame length in samples
     */
    unsigned int frame_length() { return d_frame_length; }

    /*!
     * Returns a boolean vector of size $carriers that reflects which carriers should be
     * used.
     */
    const std::vector<bool> &carrier_mask() { return d_carrier_mask; }

    ofdmradar_params(unsigned int carriers,
                     unsigned int symbols,
                     unsigned int cyclic_prefix_length,
                     unsigned int dc_guard,
                     unsigned int nyquist_guard,
                     std::vector<gr_complex> constellation,
                     unsigned int seed);

    /*!
     * \brief Allocate new ofdm radar system parameters
     *
     * \param carriers  Size of the main OFDM FFT, not all carriers may be allocated
     * \param symbols   How many symbols are used per frame
     * \param cyclic_prefix_length  Cyclic prefix in samples
     * \param dc_guard  How many carriers around DC are left unused. The total count of
     *                  empty carriers at DC is (2 * dc_guard + 1)
     * \param nyquist_guard How many carriers around the positive and negative
     * \param constellation A list of symbols comprising the constellation
     * \param seed      Random number generator seed
     */
    static sptr make(unsigned int carriers,
                     unsigned int symbols,
                     unsigned int cyclic_prefix_length,
                     unsigned int dc_guard,
                     unsigned int nyquist_guard,
                     std::vector<gr_complex> constellation,
                     unsigned int seed);

    std::string python_str();

    std::string python_repr();
};

/*!
 * This enum identifies modulation schemes which are supported by get_constellation.
 */
enum class OFDMRADAR_API modulation_scheme { BPSK, QPSK, PSK, QAM };

/*!
 * Get individual symbols (constellation) of a modulation scheme.
 *
 * \param scheme The modulation scheme to use, supported options are:
 *               BPSK, QPSK, PSK, QAM
 * \param order  For modulation schemes with a variable order (PSK & QAM), it can be
 *               specified here.
 */
std::vector<gr_complex> OFDMRADAR_API get_constellation(modulation_scheme scheme,
                                                        int order = -1);

} // namespace ofdmradar
} // namespace gr

#endif /* INCLUDED_OFDMRADAR_OFDMRADAR_H */
