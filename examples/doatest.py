#!/usr/bin/env python3
# -*- coding: utf-8 -*-

#
# SPDX-License-Identifier: GPL-3.0
#
# GNU Radio Python Flow Graph
# Title: Not titled yet
# Author: david
# GNU Radio version: v3.10.0.0git-507-geef856ed

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
from gnuradio import qtgui
import sip
from gnuradio import blocks
from gnuradio import gr
from gnuradio.filter import firdes
from gnuradio.fft import window
import sys
import signal
from argparse import ArgumentParser
from gnuradio.eng_arg import eng_float, intx
from gnuradio import eng_notation
from gnuradio.qtgui import Range, RangeWidget
from PyQt5 import QtCore
import doatest_test_waveform_gen as test_waveform_gen  # embedded python module
import numpy as np
import ofdmradar



from gnuradio import qtgui

class doatest(gr.top_block, Qt.QWidget):

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

        self.settings = Qt.QSettings("GNU Radio", "doatest")

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
        self.targets = targets = 1
        self.samples = samples = 1024
        self.samp_rate = samp_rate = 1000000
        self.output_resolution = output_resolution = 180
        self.array_size = array_size = 4
        self.alpha = alpha = 0
        self.SNR = SNR = 0

        ##################################################
        # Blocks
        ##################################################
        self._alpha_range = Range(-90, 90, 1, 0, 200)
        self._alpha_win = RangeWidget(self._alpha_range, self.set_alpha, 'alpha', "counter_slider", float, QtCore.Qt.Horizontal)
        self.top_layout.addWidget(self._alpha_win)
        self.qtgui_vector_sink_f_0 = qtgui.vector_sink_f(
            output_resolution,
            -90,
            180 / output_resolution,
            "x-Axis",
            "y-Axis",
            "",
            1, # Number of inputs
            None # parent
        )
        self.qtgui_vector_sink_f_0.set_update_time(0.10)
        self.qtgui_vector_sink_f_0.set_y_axis(0, 1.1)
        self.qtgui_vector_sink_f_0.enable_autoscale(False)
        self.qtgui_vector_sink_f_0.enable_grid(False)
        self.qtgui_vector_sink_f_0.set_x_axis_units("")
        self.qtgui_vector_sink_f_0.set_y_axis_units("")
        self.qtgui_vector_sink_f_0.set_ref_level(0)

        labels = ['', '', '', '', '',
            '', '', '', '', '']
        widths = [1, 1, 1, 1, 1,
            1, 1, 1, 1, 1]
        colors = ["blue", "red", "green", "black", "cyan",
            "magenta", "yellow", "dark red", "dark green", "dark blue"]
        alphas = [1.0, 1.0, 1.0, 1.0, 1.0,
            1.0, 1.0, 1.0, 1.0, 1.0]

        for i in range(1):
            if len(labels[i]) == 0:
                self.qtgui_vector_sink_f_0.set_line_label(i, "Data {0}".format(i))
            else:
                self.qtgui_vector_sink_f_0.set_line_label(i, labels[i])
            self.qtgui_vector_sink_f_0.set_line_width(i, widths[i])
            self.qtgui_vector_sink_f_0.set_line_color(i, colors[i])
            self.qtgui_vector_sink_f_0.set_line_alpha(i, alphas[i])

        self._qtgui_vector_sink_f_0_win = sip.wrapinstance(self.qtgui_vector_sink_f_0.pyqwidget(), Qt.QWidget)
        self.top_layout.addWidget(self._qtgui_vector_sink_f_0_win)
        self.qtgui_compass_0 = self._qtgui_compass_0_win = qtgui.GrCompass('', 250, 0.10, False, 1,False,1,"default")
        self._qtgui_compass_0_win.setColors("default","red", "black", "black")
        self._qtgui_compass_0 = self._qtgui_compass_0_win
        self.top_layout.addWidget(self._qtgui_compass_0_win)
        self.ofdmradar_array_music_0 = ofdmradar.array_music(array_size, output_resolution, targets)
        self.ofdmradar_array_esprit_0 = ofdmradar.array_esprit(array_size, targets)
        self.ofdmradar_array_corr_0 = ofdmradar.array_corr(array_size, samples)
        self.ofdmradar_array_calib_0 = ofdmradar.array_calib(array_size, 1, 0)
        self.esprit_angle_sink = qtgui.number_sink(
            gr.sizeof_float,
            0,
            qtgui.NUM_GRAPH_HORIZ,
            1,
            None # parent
        )
        self.esprit_angle_sink.set_update_time(0.10)
        self.esprit_angle_sink.set_title("ESPRIT Angle")

        labels = ['', '', '', '', '',
            '', '', '', '', '']
        units = ['', '', '', '', '',
            '', '', '', '', '']
        colors = [("black", "black"), ("black", "black"), ("black", "black"), ("black", "black"), ("black", "black"),
            ("black", "black"), ("black", "black"), ("black", "black"), ("black", "black"), ("black", "black")]
        factor = [1, 1, 1, 1, 1,
            1, 1, 1, 1, 1]

        for i in range(1):
            self.esprit_angle_sink.set_min(i, -90)
            self.esprit_angle_sink.set_max(i, 90)
            self.esprit_angle_sink.set_color(i, colors[i][0], colors[i][1])
            if len(labels[i]) == 0:
                self.esprit_angle_sink.set_label(i, "Data {0}".format(i))
            else:
                self.esprit_angle_sink.set_label(i, labels[i])
            self.esprit_angle_sink.set_unit(i, units[i])
            self.esprit_angle_sink.set_factor(i, factor[i])

        self.esprit_angle_sink.enable_autoscale(False)
        self._esprit_angle_sink_win = sip.wrapinstance(self.esprit_angle_sink.pyqwidget(), Qt.QWidget)
        self.top_layout.addWidget(self._esprit_angle_sink_win)
        self.blocks_vector_source_x_0 = blocks.vector_source_c(test_waveform_gen.get_waveform(array_size, samples * 8, alpha / 180 * np.pi, SNR, calib=False), True, array_size, [])
        self.blocks_multiply_const_xx_0 = blocks.multiply_const_ff(180 / np.pi, 1)
        self.blocks_message_debug_0 = blocks.message_debug(True)
        self.array_calib_button = _array_calib_button_toggle_button = qtgui.MsgPushButton('Calibrate Array', 'pressed',True,"default","default")
        self.array_calib_button = _array_calib_button_toggle_button

        self.top_layout.addWidget(_array_calib_button_toggle_button)



        ##################################################
        # Connections
        ##################################################
        self.msg_connect((self.array_calib_button, 'pressed'), (self.ofdmradar_array_calib_0, 'trigger'))
        self.msg_connect((self.ofdmradar_array_calib_0, 'calib'), (self.ofdmradar_array_corr_0, 'calib'))
        self.connect((self.blocks_multiply_const_xx_0, 0), (self.esprit_angle_sink, 0))
        self.connect((self.blocks_multiply_const_xx_0, 0), (self.qtgui_compass_0, 0))
        self.connect((self.blocks_vector_source_x_0, 0), (self.ofdmradar_array_corr_0, 0))
        self.connect((self.ofdmradar_array_corr_0, 0), (self.ofdmradar_array_calib_0, 0))
        self.connect((self.ofdmradar_array_corr_0, 0), (self.ofdmradar_array_esprit_0, 0))
        self.connect((self.ofdmradar_array_corr_0, 0), (self.ofdmradar_array_music_0, 0))
        self.connect((self.ofdmradar_array_esprit_0, 0), (self.blocks_multiply_const_xx_0, 0))
        self.connect((self.ofdmradar_array_music_0, 0), (self.qtgui_vector_sink_f_0, 0))


    def closeEvent(self, event):
        self.settings = Qt.QSettings("GNU Radio", "doatest")
        self.settings.setValue("geometry", self.saveGeometry())
        self.stop()
        self.wait()

        event.accept()

    def get_targets(self):
        return self.targets

    def set_targets(self, targets):
        self.targets = targets

    def get_samples(self):
        return self.samples

    def set_samples(self, samples):
        self.samples = samples
        self.blocks_vector_source_x_0.set_data(test_waveform_gen.get_waveform(self.array_size, self.samples * 8, self.alpha / 180 * np.pi, self.SNR, calib=False), [])

    def get_samp_rate(self):
        return self.samp_rate

    def set_samp_rate(self, samp_rate):
        self.samp_rate = samp_rate

    def get_output_resolution(self):
        return self.output_resolution

    def set_output_resolution(self, output_resolution):
        self.output_resolution = output_resolution
        self.qtgui_vector_sink_f_0.set_x_axis(-90, 180 / self.output_resolution)

    def get_array_size(self):
        return self.array_size

    def set_array_size(self, array_size):
        self.array_size = array_size
        self.blocks_vector_source_x_0.set_data(test_waveform_gen.get_waveform(self.array_size, self.samples * 8, self.alpha / 180 * np.pi, self.SNR, calib=False), [])

    def get_alpha(self):
        return self.alpha

    def set_alpha(self, alpha):
        self.alpha = alpha
        self.blocks_vector_source_x_0.set_data(test_waveform_gen.get_waveform(self.array_size, self.samples * 8, self.alpha / 180 * np.pi, self.SNR, calib=False), [])

    def get_SNR(self):
        return self.SNR

    def set_SNR(self, SNR):
        self.SNR = SNR
        self.blocks_vector_source_x_0.set_data(test_waveform_gen.get_waveform(self.array_size, self.samples * 8, self.alpha / 180 * np.pi, self.SNR, calib=False), [])




def main(top_block_cls=doatest, options=None):

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
