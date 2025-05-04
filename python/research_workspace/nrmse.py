#!/usr/bin/env python3

'''
　リザバーコンピューティングの3.7.1より、RMSEを計算するプログラム。
rmse.py y.mat d.mat
　というように使う。
'''

import sys
import numpy as np

import ndarray

def rmse(mat1, mat2):
    if mat1.shape != mat2.shape:
        print("__main__: mat1.shape != mat2.shape")
        exit()

    sum = 0
    for i in range(mat1.shape[1]):
        sum += ndarray.norm(mat1[:, i] - mat2[:, i]) ** 2
        # sum += (mat1[:, i] - mat2[:, i]) @ (mat1[:, i] - mat2[:, i])

    return (sum / mat1.shape[1]) ** (1 / 2)


def nrmse(mat_y, mat_d):
    return rmse(mat_y, mat_d) / ndarray.var(mat_d)

if __name__ == "__main__":

    if len(sys.argv) != 3:
        print("\
\n\
Usage: rmse.py y.mat d.mat\n\
__main__: len(sys.argv) != 3"
        )
        exit()

    y = ndarray.read(sys.argv[1])
    d = ndarray.read(sys.argv[2])

    print('{:.5f}'.format(nrmse(y, d)))
    
