#!/usr/bin/env python3

import numpy as np
import matplotlib.pyplot as plt
from matplotlib.colors import LinearSegmentedColormap
import sys

import matplotlib as mpl
mpl.rcParams['text.usetex'] = True

from matrix import matrix

help=str(
    "\n"
    "Usage: monochrome_map.py truth_links.fmat truth_links.pdf"
    "\n"
)

if len(sys.argv) != 3:
    print(help)
    exit(1)

mat = matrix(sys.argv[1])

# [4,9,14,19,24,...]と["5","10","15","20",...]という配列を作る。
tic=[i+4 for i in range(mat.shape[1]) if i % 5 == 0]
label=[f"{i+1}" for i in tic]

plt.figure(figsize=(4, 4))
plt.xticks(tic, label)
plt.yticks(tic, label)
plt.xlabel("Node index $i$", fontsize=12, fontname="Times New Roman", labelpad=1)
plt.ylabel("Node index $j$", fontsize=12, fontname="Times New Roman", labelpad=1)

for i in range(1, 38, 2):
    plt.axhline(y=0.5 * i, color=(0,0,0), linewidth=0.8)
    plt.axvline(x=0.5 * i, color=(0,0,0), linewidth=0.8)

# set palette defined (0 "rgb(1,1,1)", 1.0 "rgb(0,0,0))と同じ。
cmap = LinearSegmentedColormap.from_list("custom_gray", [(0, (1,1,1)), (1, (0,0,0))])

# plot
plt.imshow(mat, cmap=cmap, vmin=0, vmax=1)
# save
plt.savefig(sys.argv[2], format="pdf")



'''
#!/usr/bin/env gnuplot

set terminal pdf size 4,4
set output "akl.pdf"

set tics font "Times New Roman, 10"

set size ratio 1
#set margin 0,-10,4,1
#set bmargin 4

set xlabel "Node index i" font "Times New Roman, 18"
set ylabel "Node index j" font "Times New Roman, 18"

set xrange [-0.5:19.5]
set yrange [19.5:-0.5]

set cbrange [0:1]
unset colorbox
set palette defined (0 "#FFFFFF", 1 "#000000")
do for [i=1:38:2] {
    set arrow from -0.5,0.5*i to 19.5,0.5*i nohead lc rgb "black" lw 3
    set arrow from 0.5*i,-0.5 to 0.5*i,19.5 nohead lc rgb "black" lw 3
}

plot "akl.dat" using 2:1:3 with points pt 5 ps 1.8 lc palette notitle
'''
