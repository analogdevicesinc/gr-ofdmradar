import numpy as np

class Constellation:
    def __init__(self, scheme, order=-1):
        assert type(order) == int and int(order) == order, "Can only handle integer orders"
        self.order = int(order)

        self.scheme = scheme.upper()

        def normalize_constellation(constellation):
            no_offset = constellation - np.sum(constellation) / len(constellation)
            avg_energy = np.sum(no_offset * np.conj(no_offset)) / len(constellation)
            return no_offset / np.sqrt(avg_energy)

        if self.scheme == "QAM":
            assert order > 0, "No valid order provided!"
            power = np.log2(order)
            assert power == int(power) and int(power) & 1 == 0, "Constellation order has to be in {2^{2*n} for n in N}"
            self.power = int(power)

            x = np.arange(2**(self.power >> 1))
            self.symbols = normalize_constellation((x[:,np.newaxis] + 1j * x[np.newaxis,:]).reshape(len(x)*len(x)))

        elif self.scheme == "BPSK":
            self.power = 1
            self.order = 2
            self.symbols = np.array([1, -1], dtype=complex)

        else:
            raise NameError("Unknown scheme!")

        self.mods = 2 ** (self.power - 1 - np.arange(self.power))

    def modulate_data(self, data):
        assert np.all(data == data.astype(int)), \
            "data should be an array of integers, but contains other stuff: " \
            + str(data[np.where(data != data.astype(int))])
        data = data.astype(int)

        assert np.all(data >= 0) and np.all(data < self.order), "Input data out of range, should be within [0, order)!"

        return self.symbols[data]

    def modulate_bits(self, data):
        assert np.all(data == data.astype(bool)), \
            "data should be an array of ones and zeros, but contains other stuff: " \
            + str(data[np.where(data != data.astype(bool))])

        assert len(data) % self.power == 0, "The length of the data vector needs to be divisble by " + str(self.power)
        rows = len(data) // self.power
        bits = data.astype(int).reshape((rows, self.power))
        return self.modulate_data(np.sum(bits * self.mods[np.newaxis,:], axis=1))

