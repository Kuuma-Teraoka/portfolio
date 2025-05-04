#!/usr/bin/env python3

'''
　stcd.fmatからは振動子20状態変数3なのか、振動子30状態変数2なのかは判別不可能。最初にnode=20と書いておいて、状態変数の数であるdimををshape[0] / nodeで自動的に決定するようにする。
'''

help=str(
    "\n"
    "Usage: stcd_color_map.py stcd.fmat stcd.pdf"
    "\n"
)


import numpy as np
import matplotlib.pyplot as plt
from matplotlib.colors import LinearSegmentedColormap
import sys

import matplotlib as mpl
mpl.rcParams['text.usetex'] = True

from matrix import matrix


node = 20

if len(sys.argv) != 3:
    print(help)
    exit(1)

mat = matrix(sys.argv[1])
dim = mat.shape[0] / node

# (012)(345)...で5番目の真ん中に5といれるので13:"5"としたい。(123)(456)なら3*5-1に"5"。dim=2なら(12)(34)で2*5-0.5。3なら中央は2なので-1、2なら中央は1と2の間なので-0.5、1なら-0、4なら2と3の間なので-1.5、10なら5と6の間なので-4.5。
tic = [i for i in range(1,mat.shape[1]+1) if i % 5 == 0] # 5,10,15
label = [f"{i}" for i in tic]
tic = [dim * i - 0.5 * (dim - 1) - 1 for i in tic]

plt.figure(figsize=(4, 4))
plt.xticks(tic, label, fontsize=8)
plt.yticks(tic, label, fontsize=8)
plt.xlabel("Node index $i$", fontsize=12, fontname="Times New Roman", labelpad=1)
plt.ylabel("Node index $j$", fontsize=12, fontname="Times New Roman", labelpad=1)

# 3であれば2.5, 5.5, 8.5, ..., 56.5に線をひく。3 * i - 0.5 for i in range(20)といったところか。
for i in range(node):
    plt.axhline(y=dim * i - 0.5, color=(1,1,1), linewidth=0.5)
    plt.axvline(x=dim * i - 0.5, color=(1,1,1), linewidth=0.5)

cmap = LinearSegmentedColormap.from_list("custom_gray", [(0, (0,0,0)), (1, (1,1,1))])

plt.imshow(mat, cmap=cmap, vmin=1.5, vmax=5)

cbar = plt.colorbar(shrink=0.8)
cbar.set_label("$S(z^{(j)}_k, z^{(i)}_l)$")
ctic = cbar.ax.get_yticks()
cticlabel = [f"{t:.1f}" for t in ctic]
cticlabel[0] = "$<1.5$"
cticlabel[-1] = "$>5.0$"

cbar.set_ticks(ctic)
cbar.set_ticklabels(cticlabel) 

plt.savefig(sys.argv[2], format="pdf")



'''
tic = [i for i in range(1,mat.shape[1]+1) if i % 5 == 0] # 5,10,15
xlabel = [f"{i}" for i in tic]
ylabel = [f"{i}" for i in tic]
tic = [3 * i - 2 for i in tic]
tic.extend([0,4,8])
xlabel.extend(["$z^{(i)}_1$","$z^{(i)}_2$","$z^{(i)}_3$"])
ylabel.extend(["$z^{(j)}_1$","$z^{(j)}_2$","$z^{(j)}_3$"])
'''


'''
#!/usr/bin/env gnuplot

set terminal pdf size 4,4
set output "output.pdf"

set tics font "Times New Roman, 10"

set size ratio 1

set xlabel "Node index i" font "Times New Roman, 18"
set ylabel "Node index j" font "Times New Roman, 18"

set xrange [-0.5:19.5]
set yrange [19.5:-0.5]

set cbrange [1.5:5]
set cbtics add (">5" 5)
set palette defined (0 "#000000", 1 "#FFFFFF")
set cblabel 'f_{ji}' font "Times New Roman, 20"

do for [i=1:38:2] {
    set arrow from -0.5,0.5*i to 19.5,0.5*i nohead front lw 1 lc rgb "#FFFFFF"
    set arrow from 0.5*i,-0.5 to 0.5*i,19.5 nohead front lw 1 lc rgb "#FFFFFF"
}

# /7->/3, $-3->$-1
plot "input.dat" using (($2-1)/3):(($1-1)/3):3 with points pt 5 ps 1.5 / 3 lc palette notitle
'''
