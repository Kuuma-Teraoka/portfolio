#!/usr/bin/env python3

help=str(
    "\n"
    "Usage: print_matrix [OPTIONS] MATRIX_FILE\n"
    "\n"
    "print_matrix a.mat\n"
    "　で行と列をそのまま' '区切りで標準出力。\n"
    "print_matrix -t a.mat\n"
    "　で転地して出力。\n"
    "print_matrix -t -s : 3:4 a.mat\n"
    "　でa[:, 3:4]を転地して出力。a.T[:, 3:4]ではなく、a[:, 3:4].Tであることに注意。\n"
    "print_matrix -o fkl.mat -s ::3 1::3 stcd.mat\n"
    "　でstcd[::3,1::3]をdat形式ではなくmat形式でfkl.matに出力。バイナリを標準出力するとバグるのでバイナリモードを追加するのではなく、出力先を決めるとバイナリで出力するようにした。\n"
    "print_matrix -d a.mat\n"
    "　で\n"
    "2 3\n"
    "9 1\n"
    "　というような行列から\n"
    "0 0 2\n"
    "0 1 3\n"
    "1 0 9\n"
    "1 1 1\n"
    "　というテキストを標準出力をする。"
    "\n"
    "__main__: len(sys.argv) - i != 1"
) 

import numpy as np
import sys

import matrix
import strings

d = False
t = False
row_slice = slice(None)
col_slice = slice(None)
output_file = None

i = 1
while i < len(sys.argv) - 1:
    if sys.argv[i] == "-d":
        d = True
    elif sys.argv[i] == "-t":
        t = True 
    elif sys.argv[i] == "-s" and i + 2 < len(sys.argv):
        i += 1
        row_slice = strings.__slice__(sys.argv[i])
        i += 1
        col_slice = strings.__slice__(sys.argv[i]) 
    elif sys.argv[i] == "-o" and i + 1 < len(sys.argv):
        i += 1
        output_file = sys.argv[i]
    else:
        break
    i += 1

if len(sys.argv) - i != 1:
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


if d:
    for i in range(mat.shape[0]):
        for j in range(mat.shape[1]):
            print(f"{i} {j} {mat[i, j]}")
else:
    if output_file == None:
        print(mat)
    else:
        mat.write(mat, output_file)





