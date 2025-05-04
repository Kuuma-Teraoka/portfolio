#!/usr/bin/env python3

import matplotlib.pyplot as plt
import numpy as np
import sys

from matrix import matrix

help=(
    "\n"
    "Usage: stcd_histogram.py f_value.dat histogram.pdf"
    "\n"
)

if len(sys.argv) != 3:
    print(help)
    exit(1)

mat = np.loadtxt(sys.argv[1])

# 0.0, 1.0, 0.01だと0.7が0.69999になるからか[0.7,0.71)が0になってしまう。なので、0.00999開始で0.699も0.7も[0.6999,0.709999)に含まれるようにする。
plt.hist(mat.flatten(), bins=np.arange(0.0099,1.01,0.01), color=(0.5,0.5,0.5), alpha=1.0, edgecolor="#000000")

plt.xlabel("F value")
plt.ylabel("Frequency")
plt.xlim(0.0,1.01)

default_yticks = plt.gca().get_yticks()
integer_yticks = [int(tick) for tick in default_yticks if tick.is_integer()]
plt.yticks(integer_yticks)



plt.savefig(sys.argv[2], format="pdf")

