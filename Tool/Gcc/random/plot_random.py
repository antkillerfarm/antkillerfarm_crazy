import matplotlib.pyplot as plt
import numpy as np

def histogram(data, n_bins, cumulative=False, x_label = "", y_label = "", title = ""):
    _, ax = plt.subplots()
    ax.hist(data, n_bins, cumulative = cumulative, color = '#539caf')
    ax.set_ylabel(y_label)
    ax.set_xlabel(x_label)
    ax.set_title(title)


n1 = np.loadtxt("./1.txt", dtype=np.float32)
histogram(n1,100)
plt.show()