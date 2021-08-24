from .constellation import Constellation
import numpy as np
from scipy import signal

class OFDMRadar:
    def __init__(self, N: int, N_guard: int, N_border_margin: int, M: int, const: Constellation):
        self.N = N
        self.N_guard = N_guard
        self.M = M
        self.const = const

        # self.excluded_carriers = np.zeros((N_border_margin << 1) + 3, dtype=int)
        # self.excluded_carriers[3:] = (N >> 1) - N_border_margin + np.arange(N_border_margin << 1)
        # self.excluded_carriers[1] = 1
        # self.excluded_carriers[2] = -1
        self.excluded_carriers = np.zeros((N_border_margin << 1), dtype=int)
        self.excluded_carriers[:] = (N >> 1) - N_border_margin + np.arange(N_border_margin << 1)
        self.carrier_mask = np.ones(N, dtype=int)
        self.carrier_mask[self.excluded_carriers] = 0
        self.active_carriers = np.arange(self.N)[self.carrier_mask]

    def forward(self, symbols):
        assert symbols.shape[-1] == self.N, "The length of the last axis must be " + str(self.N) + " samples!"
        s = np.empty(symbols.shape[:-1] + (self.N + self.N_guard,), dtype=complex)

        s[...,self.N_guard:] = np.fft.ifft(symbols * self.carrier_mask)
        s[...,:self.N_guard] = s[...,-self.N_guard:]
        return s

    def backward(self, data):
        assert data.shape[-1] == self.N + self.N_guard, "The length of the last axis must be " + str(self.N + self.N_guard) + " samples!"
        return np.fft.fft(data[...,self.N_guard:])

    def get_frame_data(self, seed: int):
        np.random.seed(seed)
        return np.random.randint(self.const.order, size=(self.M, self.N))

    def generate_random_frame(self, seed: int):
        data = self.get_frame_data(seed)
        mod = self.const.modulate_data(data)
        return mod, self.forward(mod).reshape((self.N + self.N_guard) * self.M)

    def receive_frame(self, frame):
        # "Removing the cyclic prefix", assuming that tau < t_guard!
        frame = frame.reshape((self.M, self.N + self.N_guard))
        return self.backward(frame)

    def estimate_channel(self, F_tx, frame):
        F_rx = self.receive_frame(frame)
        F_tx[:,self.excluded_carriers] = 1
        F = F_rx / F_tx
        F *= np.fft.fftshift(signal.windows.hamming(self.N)[np.newaxis, :])
        # F *= signal.windows.hamming(self.N)[np.newaxis, :]
        F[:,self.excluded_carriers] = 0
        return F

    def periodogram(self, F_tx, frame):
        estimate = self.estimate_channel(F_tx, frame)

        estimate = np.fft.fft(estimate, axis=0)
        estimate = np.fft.ifft(estimate, axis=1)
        estimate = np.fft.fftshift(estimate, axes=[0])
        return np.transpose(estimate)
