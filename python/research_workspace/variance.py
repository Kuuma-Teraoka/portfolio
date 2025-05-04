#!/usr/bin/env python3

'''
　lozの分散や平均を計算するため。nrmse.pyの副産物
variance.py loz.mat
　というように使う。
'''

import sys

import ndarray

if len(sys.argv) != 2:
    print("\
\n\
Usage: variance.py loz.mat\n\
\n\
__main__: len(sys.argv) != 2"
    )
    exit()

d = ndarray.read(sys.argv[1])

# print(ndarray.ave(d))
print('{:.5f}'.format(ndarray.var(d)))
