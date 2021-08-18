/* -*- c++ -*- */
/*
 * Copyright 2021 Analog Devices Inc.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "array_esprit_impl.h"
#include <gnuradio/io_signature.h>

namespace gr {
namespace ofdmradar {

#pragma message("set the following appropriately and remove this warning")
using input_type = float;
#pragma message("set the following appropriately and remove this warning")
using output_type = float;
array_esprit::sptr array_esprit::make()
{
    return gnuradio::make_block_sptr<array_esprit_impl>();
}


/*
 * The private constructor
 */
array_esprit_impl::array_esprit_impl()
    : gr::sync_block("array_esprit",
                     gr::io_signature::make(
                         1 /* min inputs */, 1 /* max inputs */, sizeof(input_type)),
                     gr::io_signature::make(
                         1 /* min outputs */, 1 /*max outputs */, sizeof(output_type)))
{
}

/*
 * Our virtual destructor.
 */
array_esprit_impl::~array_esprit_impl() {}

int array_esprit_impl::work(int noutput_items,
                            gr_vector_const_void_star &input_items,
                            gr_vector_void_star &output_items)
{
    const input_type *in = reinterpret_cast<const input_type *>(input_items[0]);
    output_type *out = reinterpret_cast<output_type *>(output_items[0]);

#pragma message("Implement the signal processing in your block and remove this warning")
    // Do <+signal processing+>

    // Tell runtime system how many output items we produced.
    return noutput_items;
}

} /* namespace ofdmradar */
} /* namespace gr */
