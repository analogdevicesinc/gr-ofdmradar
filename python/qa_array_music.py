#!/usr/bin/env python
# -*- coding: utf-8 -*-
#
# Copyright 2021 Analog Devices Inc.
#
# SPDX-License-Identifier: GPL-3.0-or-later
#

from gnuradio import gr, gr_unittest, blocks
import numpy as np
# from gnuradio import blocks
try:
    from ofdmradar import array_music, array_corr
except ImportError:
    import os
    import sys
    dirname, filename = os.path.split(os.path.abspath(__file__))
    d = os.path.join(dirname, "bindings")
    sys.path.append(d)
    from ofdmradar import array_music, array_corr

class qa_array_music(gr_unittest.TestCase):

    def setUp(self):
        self.tb = gr.top_block()

    def tearDown(self):
        self.tb = None

    def test_instance(self):
        elements = 4
        N = 1024
        output_resolution = 1024
        s = np.ones((elements, N)) # np.exp(1j * 0.1 * 2*np.pi * np.arange(elements))
        N_0 = 1
        sigma = np.sqrt(N_0/2)
        n = sigma * (np.random.randn(elements, N) + 1j * np.random.randn(elements, N))
        x = n + s
        omega = 0.1
        steer = np.ones(elements) # np.exp(1j * 2*np.pi * omega * np.arange(elements))
        x = (x * steer[:,np.newaxis])
        x = np.repeat(x, 2)

        self.vector_source = blocks.vector_source_c(x, repeat=False, vlen=elements)
        self.corr_instance = array_corr(elements, N)
        self.tb.connect((self.vector_source, 0), (self.corr_instance, 0))

        self.music_instance = array_music(elements, output_resolution, 1)
        self.tb.connect((self.corr_instance, 0), (self.music_instance, 0))
        self.nullsink = blocks.null_sink(gr.sizeof_float * output_resolution);
        self.tb.connect((self.music_instance, 0), (self.nullsink, 0))

        self.tb.run()

        assert False

if __name__ == '__main__':
    gr_unittest.run(qa_array_music)
