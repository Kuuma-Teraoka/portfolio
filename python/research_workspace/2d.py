#!/usr/bin/env python3



help=str(
    "\n"
    "Usage: attractor MATRIX_FILE OUTPUT_FILE\n"
    "\n"
)

import sys
import matplotlib.pyplot as plt

import matrix

if len(sys.argv) != 3:
    print(help, end='')
    exit()

mat = matrix.matrix(sys.argv[1])
mat = mat[:, 40000:41000]

plt.figure(figsize=(4, 3))
plt.plot(mat[0, :], mat[1, :], color='#000000', linewidth=0.5)
plt.tick_params(axis="x", direction="in")
plt.tick_params(axis="y", direction="in")





plt.savefig(sys.argv[2], format="pdf")

