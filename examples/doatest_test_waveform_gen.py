import numpy as np

phi = None

def get_waveform(elements, N, alpha, SNR_dB, calib=True):
    global phi

    if phi is None:
        phi = np.exp(1j * 2*np.pi * np.random.randn(elements))

    if calib:
        s = np.ones((elements, N)) # np.exp(1j * 0.1 * 2*np.pi * np.arange(elements))
    else:
        s = np.ones((elements, N)) * phi[:,np.newaxis]

    SNR = 10 ** (SNR_dB/10)
    N_0 = 1/SNR
    sigma = np.sqrt(N_0/2)
    n = sigma * (np.random.randn(elements, N) + 1j * np.random.randn(elements, N))

    omega = np.sin(alpha)
    d = np.arange(elements, dtype=float)/2
    steer = np.exp(1j * 2*np.pi * omega * d)

    x = np.tile((s * steer[:,np.newaxis]) + n, 2)
    return np.transpose(x).reshape(np.prod(x.shape))

