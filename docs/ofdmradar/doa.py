import numpy as np
from scipy import constants
from numpy import linalg as lin

class SingleToneReceiverArray:
    """
    Used to simulate what a linear array receiver in free space might see
    in respose to a single-tone transmission
    """

    def __init__(self, f_c, f_ppm, f_s, array_size, element_spacing, SNR=15, calibrated=True):
        self.f_c = f_c
        self.f_r = f_c * (1 + np.random.randn() * (f_ppm/2e6))
        self.f_ppm = f_ppm
        self.f_s = f_s
        self.array_size = array_size
        self.element_spacing = element_spacing
        self.lmb = constants.c / f_c
        self.SNR = 10 ** (SNR/10)

        if calibrated:
            self.Gamma = np.ones(4)
        else:
            self.Gamma = np.exp(1j * 2*np.pi * np.random.rand(4)) * (0.9+0.2*np.random.rand(4))

    def get_phasors(self, angle):
        """

               [tx]
                 \
                  \
                   \
        ============================ (Far field, incoming wave is plane)
                (    \
               (pi/2+a\
               (-------\
               [rx] [rx] [rx] [rx]

        """
        positions = (np.arange(self.array_size) - (self.array_size-1)/2) * self.element_spacing
        d = positions * np.sin(-angle)

        return np.exp(1j*2*np.pi * d  / self.lmb)

    def receive(self, angle, N):
        delta_f = self.f_r - self.f_c
        t = np.arange(N) / self.f_s

        sigma_h = np.sqrt(1/(2*self.SNR))
        return np.exp(1j * 2*np.pi* delta_f*t)[np.newaxis,:] \
            * (self.get_phasors(angle)*self.Gamma)[:,np.newaxis] \
            + (np.random.randn(self.array_size, N) \
                + 1j*np.random.randn(self.array_size, N)) * sigma_h


def angle_from_omega(omega, delta_x, lmb):
    return np.arcsin(-omega / (2*np.pi) * lmb / delta_x)

def omega_from_angle(phi, delta_x, lmb):
    return 2*np.pi * np.sin(-phi) * delta_x / lmb

def get_sn_subspaces(r, n, cal=None):
    assert len(r.shape) == 2
    m,N = r.shape

    R_x = r @ np.conj(r.T) / N

    if not cal is None:
        R_x *= cal

    # Determine signal and noise spaces
    vals, vecs = lin.eigh(R_x)
    s_i = []
    n_i = []
    i = 0
    for k,v in sorted(enumerate(np.abs(vals)), key=lambda x: x[1], reverse=True):
        if i < n:
            s_i.append(k)
        else:
            n_i.append(k)

        i += 1

    svals, svecs = vals[s_i], vecs[:,s_i] # Signal Space
    nvals, nvecs = vals[n_i], vecs[:,n_i] # Noise Space

    return (svals, svecs), (nvals, nvecs)

def P_MU(omega, M, nvecs):
    e = np.exp(1j * omega * np.arange(M))
    # return 1 / np.sum(np.abs(np.conj(e.T) @ nvecs.T)**2)
    res = 1 / (np.conj(e) @ nvecs @ np.conj(nvecs.T) @ e)
    assert np.prod(np.array(res.shape)) == 1
    return np.sum(res)

def esprit(r, n, cal=None):
    """
    r: Receive signal, [m x s]
    n: Targets
    m: Array size
    """
    assert len(r.shape) == 2
    m,N = r.shape

    (svals, svecs), (nvals, nvecs) = get_sn_subspaces(r, n, cal)

    s_1_ = np.zeros((m-1, m))
    s_1_[:,:m-1] = np.eye(m-1)
    S_1 = s_1_ @ svecs

    s_2_ = np.zeros((m-1,m))
    s_2_[:,1:] = np.eye(m-1)
    S_2 = s_2_ @ svecs

    PHI = lin.solve(S_2.conj().T @ S_2, S_2.conj().T @ S_1)
    chi = lin.eigvals(PHI)
    return -(angle_from_omega(np.angle(chi), 1/2, 1))

def estimate_gamma(r, rx, n, m, phi):
    (svals, svecs), (nvals, nvecs) = get_sn_subspaces(r, n, m)

    D_i = np.diag(rx.get_phasors(phi))
    W = D_i.conj() @ svecs @ svecs.T.conj() @ D_i

    vals, vecs = lin.eigh(W)

    return vecs[:,np.argmax(np.abs(vals))]
