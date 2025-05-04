#!/usr/bin/env python3

help=str(
    "\n"
    "Usage: time_series.py [OPTIONS] MATRIX_FILE OUTPUT_FILE\n"
    "\n"
)

import numpy as np
import sys

import matrix
import strings

t = False
row_slice = slice(None)
col_slice = slice(None)

i = 1
while i < len(sys.argv) - 1:
    if sys.argv[i] == "-t":
        t = True
    elif sys.argv[i] == "-s" and i + 2 < len(sys.argv):
        i += 1
        row_slice = strings.__slice__(sys.argv[i])
        i += 1
        col_slice = strings.__slice__(sys.argv[i]) 
    else:
        break
    i += 1

if len(sys.argv) - i != 2:
    print(help)
    exit()

if isinstance(row_slice, int):
    row_slice = slice(row_slice, row_slice + 1)
if isinstance(col_slice, int):
    col_slice = slice(col_slice, col_slice + 1)

mat = matrix.matrix(sys.argv[i])
mat = mat[row_slice, col_slice]
if t:
    mat = mat.T



'''
　以上はmatpri.pyとほぼ同じ
'''

import matplotlib.pyplot as plt
plt.figure(figsize=(mat.shape[1] // 500, 3))
for i in range(mat.shape[0]):
    plt.plot(mat[i, :], linewidth=0.1)

plt.xlim(0-1, mat.shape[1]+1)
plt.tick_params(axis="x", direction="in")
plt.tick_params(axis="y", direction="in")

plt.savefig(sys.argv[len(sys.argv) - 1], format="pdf")







































'''


#!/usr/bin/env gnuplot 

set terminal svg size 2400,600
set output "time_series_output_comparison.svg"

set tics font "Times New Roman, 18"
set key font "Times New Roman, 14"

set xlabel "t" font "Times New Roman, 22"
set ylabel "Output" font "Times New Roman, 22"

set xrange [0:1000]
set yrange [-50:50]

plot \
"d.dat" using 0:1 with points pt 5 ps 0.5 lc rgb "#FFA500"  title "Target", \
"y.dat" using 0:1 with linespoints pt 7 ps 0.5 lt 1 lw 0.5 lc rgb "#000000" title "Model"


'''
