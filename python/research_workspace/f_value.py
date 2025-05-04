#!/usr/bin/env python3

'''
　confusion_matrix.matを受け取って、F値を計算するプログラム。
<FP>_R = L * (380 - L) / 380
　である。これがランダムに推定したときのFPの数。詳しくはUsint...tex参照。

　Lはconfusion_matrix.matのTPとFNの和から求める。tp,tn,fp,fnと0,1,2,3の対応はconfusion_matrix.pyで定義している。といってもこの順番の通り。

20は15がTP=0で0で割ることになるので除外。あとで、15のconfusionだけ移動して計算してdatに手入力した。
list=(35 50 65 80 100 120 140 160 180 200 220 240 260 280 300 320 340 360 380)
cd ~/d/inbox/example_1/20
for i in ${list[@]}; do cd ../$i ; echo -n "${i} " >> ~/d/inbox/f_value.dat ; f_value.py */confusion_matrix.mat >> ~/d/inbox/f_value.dat ; done
'''

import sys

import ndarray

def f_value(mat):
    tp = ndarray.count(mat, 0.)
    fp = ndarray.count(mat, 2.)
    fn = ndarray.count(mat, 3.)
    
    f1 = tp / (tp + fp)
    f2 = tp / (tp + fn)
    
    f = 2 * f1 * f2 / (f1 + f2)

    return f


if __name__ == "__main__":

    if len(sys.argv) < 2:
        print("\
    \n\
    Usage: f_value.py confusion_matrix.mat\n\
    \n\
    __main__: len(sys.argv) != 2"
        )
        exit()

    elif len(sys.argv) == 2:
        mat = ndarray.read(sys.argv[1])
        print('{:.5f}'.format(f_value(mat)))

    elif len(sys.argv) > 2:
        mat_lst = []
        for i in range(len(sys.argv) - 1):
            mat_lst.append(ndarray.read(sys.argv[i + 1]))
        
        f_value_lst = [f_value(i) for i in mat_lst]

        sum = 0
        for i in f_value_lst:
            sum += i
        
        print('{:.5f}'.format(sum / len(f_value_lst)))

