#!/usr/bin/env python3

'''
　いまは[0:2, 40000:42000]にしているが、matpriのように-sを導入したい。そこで、matpriでファイル出力のときにバイナリを出力するようにして、かつこっちでは3d.py a.pdfのようにlen(sys.argv) == 2なら、標準入力からfmatを受け取るようにする。これにより、
matpri -s 0:3 40000:42000 ts.fmat | 3d.py attracter.pdf
　とできるようになる。time_seriesでも-sオブション処理の部分を消してmatpriを使うようにしたい。
'''

help=str(
    "\n"
    "Usage: attractor MATRIX_FILE OUTPUT_FILE\n"
    "\n"
)

import sys
import matplotlib.pyplot as plt

import matrix

if len(sys.argv) != 3:
    print(help)
    exit()

mat = matrix.matrix(sys.argv[1])
mat = mat[:, 20000:50000]
fig = plt.figure()
ax = fig.add_subplot(111, projection='3d')
ax.plot3D(mat[2, :], mat[1, :], mat[0, :], color="#000000", linewidth=0.5)

ax.view_init(elev=15, azim=-60)
ax.grid(False)
ax.xaxis.set_pane_color((1.0, 1.0, 1.0, 1.0))
ax.yaxis.set_pane_color((1.0, 1.0, 1.0, 1.0))
ax.zaxis.set_pane_color((0.95, 0.95, 0.95, 1.0))
ax.set_xlabel("x")
ax.set_ylabel("y")
ax.set_zlabel("z")
ax.xaxis._axinfo["tick"]["inward_factor"] = 0
ax.yaxis._axinfo["tick"]["inward_factor"] = 0
ax.zaxis._axinfo["tick"]["inward_factor"] = 0
ax.xaxis._axinfo["tick"]["outward_factor"] = 0.3
ax.yaxis._axinfo["tick"]["outward_factor"] = 0.3
ax.zaxis._axinfo["tick"]["outward_factor"] = 0.3




plt.savefig(sys.argv[2], format="pdf")

