#!/usr/bin/env python3

import matplotlib.pyplot as plt
import numpy as np
import sys
import matplotlib as mpl
mpl.rcParams['text.usetex'] = True

from matrix import matrix

help=str(
    "\n"
    "Usage: stcd_histogram.py stcd.fmat stcd_histogram.pdf"
    "\n"
)

if len(sys.argv) != 3:
    print(help)
    exit(1)

mat = matrix(sys.argv[1])
mat = mat[1::3,2::3]

plt.hist(mat.flatten(), bins=np.arange(0,5,0.1), edgecolor='black')

plt.xlabel("$S(z^{(j)}_k, z^{(i)}_l)$")
plt.ylabel("Frequency")

plt.savefig(sys.argv[2], format="pdf")

