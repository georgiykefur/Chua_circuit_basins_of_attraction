import numpy as np
import matplotlib.pyplot as plt
from matplotlib.colors import ListedColormap

inputfile = "basins.csv"
outputfile = "plots/" + inputfile + ".png"

data = np.loadtxt(inputfile, delimiter=",", skiprows=1)

u2 = np.unique(data[:, 0])
i = np.unique(data[:, 1])

labels = data[:, 4].astype(int).reshape(len(u2), len(i)).T

cmap = ListedColormap(["gray", "red", "green", "yellow"])

plt.pcolormesh(
    u2, i, labels,
    cmap=cmap,
    vmin=-0.5, vmax=3.5,
    shading="nearest"
)
ax = plt.gca()

# ax.spines["left"].set_position("zero")
# ax.spines["bottom"].set_position("zero")
ax.set_box_aspect(1) 

plt.xlabel("u2(0)")
plt.ylabel("i(0)")
plt.title("PC3, u1(0) = 3")
plt.savefig(outputfile, dpi=200)
plt.show()
