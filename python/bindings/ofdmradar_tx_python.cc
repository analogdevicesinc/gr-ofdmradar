/*
 * Copyright 2021 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

/***********************************************************************************/
/* This file is automatically generated using bindtool and can be manually edited  */
/* The following lines can be configured to regenerate this file during cmake      */
/* If manual edits are made, the following tags should be modified accordingly.    */
/* BINDTOOL_GEN_AUTOMATIC(0)                                                       */
/* BINDTOOL_USE_PYGCCXML(0)                                                        */
/* BINDTOOL_HEADER_FILE(ofdmradar_tx.h)                                            */
/* BINDTOOL_HEADER_FILE_HASH(53535eec94d43359bfac1cc8c5533879)                     */
/***********************************************************************************/

#include <pybind11/complex.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

namespace py = pybind11;

#include <ofdmradar/ofdmradar_tx.h>
// pydoc.h is automatically generated in the build directory
#include <ofdmradar_tx_pydoc.h>

void bind_ofdmradar_tx(py::module &m)
{

    using ofdmradar_tx = gr::ofdmradar::ofdmradar_tx;

    py::class_<ofdmradar_tx,
               gr::sync_block,
               gr::block,
               gr::basic_block,
               std::shared_ptr<ofdmradar_tx>>(m, "ofdmradar_tx", D(ofdmradar_tx))

        .def(py::init(&ofdmradar_tx::make),
             py::arg("ofdm_params"),
             py::arg("len_tag_key") = "packet_len",
             D(ofdmradar_tx, make))

        ;
}