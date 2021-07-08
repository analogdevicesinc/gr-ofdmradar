# this module will be imported in the into your flowgraph

import numpy as np
import matplotlib
import matplotlib.pyplot as plt

counter = 0
running_sum = np.zeros((1024, 64), dtype=float)

def handle_data(data):
    global counter,running_sum
    estimate = np.fft.fftshift(data, axes=[0])
    estimate = np.transpose(estimate)

    matplotlib.use("agg")
    plt.figure(figsize=(8, 10))
    running_sum += np.abs(estimate)[::-1,:]
    plt.imshow(running_sum, interpolation="none", aspect=1/2, extent=[-32, 31, 0, 1023])
    # plt.imshow(np.transpose(np.abs(data)))
    # plt.colorbar()
    plt.ylim([0, 200])
    plt.ylabel("Distance")
    plt.xlabel("Doppler");
    ofile = f"/tmp/periodograms/{counter:03}.png"

    plt.savefig(ofile)
    plt.close()
    counter += 1
    # print(f"Wrote: {ofile}")
