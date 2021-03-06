#!/usr/bin/env python3
# -*- coding: utf-8 -*-

#
# SPDX-License-Identifier: GPL-3.0
#
# GNU Radio Python Flow Graph
# Title: Not titled yet
# Author: david
# GNU Radio version: v3.10.0.0git-508-ga992edbc

from distutils.version import StrictVersion

if __name__ == '__main__':
    import ctypes
    import sys
    if sys.platform.startswith('linux'):
        try:
            x11 = ctypes.cdll.LoadLibrary('libX11.so')
            x11.XInitThreads()
        except:
            print("Warning: failed to XInitThreads()")

from PyQt5 import Qt
import ofdmradar
import sip
from gnuradio import analog
from gnuradio import blocks
from gnuradio import filter
from gnuradio.filter import firdes
from gnuradio import gr
from gnuradio.fft import window
import sys
import signal
from argparse import ArgumentParser
from gnuradio.eng_arg import eng_float, intx
from gnuradio import eng_notation
import numpy as np



from gnuradio import qtgui

class ofdmradar_test(gr.top_block, Qt.QWidget):

    def __init__(self):
        gr.top_block.__init__(self, "Not titled yet", catch_exceptions=True)
        Qt.QWidget.__init__(self)
        self.setWindowTitle("Not titled yet")
        qtgui.util.check_set_qss()
        try:
            self.setWindowIcon(Qt.QIcon.fromTheme('gnuradio-grc'))
        except:
            pass
        self.top_scroll_layout = Qt.QVBoxLayout()
        self.setLayout(self.top_scroll_layout)
        self.top_scroll = Qt.QScrollArea()
        self.top_scroll.setFrameStyle(Qt.QFrame.NoFrame)
        self.top_scroll_layout.addWidget(self.top_scroll)
        self.top_scroll.setWidgetResizable(True)
        self.top_widget = Qt.QWidget()
        self.top_scroll.setWidget(self.top_widget)
        self.top_layout = Qt.QVBoxLayout(self.top_widget)
        self.top_grid_layout = Qt.QGridLayout()
        self.top_layout.addLayout(self.top_grid_layout)

        self.settings = Qt.QSettings("GNU Radio", "ofdmradar_test")

        try:
            if StrictVersion(Qt.qVersion()) < StrictVersion("5.0.0"):
                self.restoreGeometry(self.settings.value("geometry").toByteArray())
            else:
                self.restoreGeometry(self.settings.value("geometry"))
        except:
            pass

        ##################################################
        # Variables
        ##################################################
        self.samp_rate = samp_rate = 250e6
        self.radar_params = radar_params = ofdmradar.ofdmradar_params(1024,
          128,
          4096,
          1024,
          128,
          1,
          64,
          window.WIN_HAMMING,
          ofdmradar.get_constellation(ofdmradar.modulation_scheme.BPSK, 4),
          0)

        ##################################################
        # Blocks
        ##################################################
        self.ofdmradar_ofdmradar_tx_0 = ofdmradar.ofdmradar_tx(radar_params, 'packet_len')
        self.ofdmradar_ofdmradar_rx_0 = ofdmradar.ofdmradar_rx(radar_params, 'packet_len', radar_params.frame_length)
        self.ofdmradar_ofdmradar_gui_0 = ofdmradar.ofdmradar_gui(radar_params, None)
        self._ofdmradar_ofdmradar_gui_0_win = sip.wrapinstance(self.ofdmradar_ofdmradar_gui_0.pyqwidget(), Qt.QWidget)
        self.top_grid_layout.addWidget(self._ofdmradar_ofdmradar_gui_0_win, 0, 1, 2, 1)
        for r in range(0, 2):
            self.top_grid_layout.setRowStretch(r, 1)
        for c in range(1, 2):
            self.top_grid_layout.setColumnStretch(c, 1)
        self.freq_xlating_fir_filter_xxx_0_0 = filter.freq_xlating_fir_filter_ccc(1, [0]*99 + [0.1] + [0]*10 + [0.4] + [0]*10, 10, samp_rate)
        self.freq_xlating_fir_filter_xxx_0 = filter.freq_xlating_fir_filter_ccc(1, [0]*50 + [0.3] + [0]*30 + [0.1] + [0]*10, 30000, samp_rate)
        self.blocks_throttle_0 = blocks.throttle(gr.sizeof_gr_complex*1, 1048576,True)
        self.blocks_multiply_const_vxx_0_0 = blocks.multiply_const_cc(10)
        self.blocks_multiply_const_vxx_0 = blocks.multiply_const_cc(1e-3)
        self.blocks_add_xx_0 = blocks.add_vcc(1)
        self.analog_fastnoise_source_x_0 = analog.fastnoise_source_c(analog.GR_GAUSSIAN, 0.1, 0, 8192)



        ##################################################
        # Connections
        ##################################################
        self.connect((self.analog_fastnoise_source_x_0, 0), (self.blocks_add_xx_0, 2))
        self.connect((self.blocks_add_xx_0, 0), (self.ofdmradar_ofdmradar_rx_0, 0))
        self.connect((self.blocks_multiply_const_vxx_0, 0), (self.blocks_throttle_0, 0))
        self.connect((self.blocks_multiply_const_vxx_0_0, 0), (self.ofdmradar_ofdmradar_gui_0, 0))
        self.connect((self.blocks_throttle_0, 0), (self.freq_xlating_fir_filter_xxx_0, 0))
        self.connect((self.blocks_throttle_0, 0), (self.freq_xlating_fir_filter_xxx_0_0, 0))
        self.connect((self.freq_xlating_fir_filter_xxx_0, 0), (self.blocks_add_xx_0, 0))
        self.connect((self.freq_xlating_fir_filter_xxx_0_0, 0), (self.blocks_add_xx_0, 1))
        self.connect((self.ofdmradar_ofdmradar_rx_0, 0), (self.blocks_multiply_const_vxx_0_0, 0))
        self.connect((self.ofdmradar_ofdmradar_tx_0, 0), (self.blocks_multiply_const_vxx_0, 0))


    def closeEvent(self, event):
        self.settings = Qt.QSettings("GNU Radio", "ofdmradar_test")
        self.settings.setValue("geometry", self.saveGeometry())
        self.stop()
        self.wait()

        event.accept()

    def get_samp_rate(self):
        return self.samp_rate

    def set_samp_rate(self, samp_rate):
        self.samp_rate = samp_rate

    def get_radar_params(self):
        return self.radar_params

    def set_radar_params(self, radar_params):
        self.radar_params = radar_params




def main(top_block_cls=ofdmradar_test, options=None):

    if StrictVersion("4.5.0") <= StrictVersion(Qt.qVersion()) < StrictVersion("5.0.0"):
        style = gr.prefs().get_string('qtgui', 'style', 'raster')
        Qt.QApplication.setGraphicsSystem(style)
    qapp = Qt.QApplication(sys.argv)

    tb = top_block_cls()

    tb.start()

    tb.show()

    def sig_handler(sig=None, frame=None):
        tb.stop()
        tb.wait()

        Qt.QApplication.quit()

    signal.signal(signal.SIGINT, sig_handler)
    signal.signal(signal.SIGTERM, sig_handler)

    timer = Qt.QTimer()
    timer.start(500)
    timer.timeout.connect(lambda: None)

    qapp.exec_()

if __name__ == '__main__':
    main()
