#!/usr/bin/env python3

'''
　confusion_matrix.matを受け取って、FIG.2(c)ででてくる、frac{FP}{<FP>_R}を求めて標準出力する関数。
<FP>_R = L * (380 - L) / 380
　である。これがランダムに推定したときのFPの数。詳しくはUsint...tex参照。

　Lはconfusion_matrix.matのTPとFNの和から求める。tp,tn,fp,fnと0,1,2,3の対応はconfusion_matrix.pyで定義している。といってもこの順番の通り。

　引数を可変長にして、2以上なら平均を出力するようにすることで、
fp_fpr.py */confusion_matrix.mat
　で簡単に平均できるようにする。

380はfprの380-380の部分が0になり、0で割ることになるので除外。
list=(20 35 50 65 80 100 120 140 160 180 200 220 240 260 280 300 320 340 360)
cd ~/d/inbox/example_1/20
for i in ${list[@]}; do cd ../$i ; echo -n "${i} " >> ~/d/inbox/fp_fpr.dat ; fp_fpr.py */confusion_matrix.mat >> ~/d/inbox/fp_fpr.dat ; done
　で.dat作れる。
'''

import sys

import ndarray

def fp_fpr(mat):
    tp = ndarray.count(mat, 0.)
    fp = ndarray.count(mat, 2.)
    fn = ndarray.count(mat, 3.)
    l = tp + fn
    n = mat.shape[0]

    fpr = l * (n**2-n - l) / (n**2-n)

    return fp, fpr


if __name__ == "__main__":

    if len(sys.argv) < 2:
        print("\
    \n\
    Usage: fp_fpr.py confusion_matrix.mat\n\
    \n\
    __main__: len(sys.argv) != 2"
        )
        exit()

    elif len(sys.argv) == 2:
        mat = ndarray.read(sys.argv[1]) 
        print('{:.5f}'.format(fp_fpr(mat)[0] / fp_fpr(mat)[1]) + ' ' +  '{:.5f}'.format(fp_fpr(mat)[0]))

    elif len(sys.argv) > 2:
        mat_lst = []
        for i in range(len(sys.argv) - 1):
            mat_lst.append(ndarray.read(sys.argv[i + 1]))
        
        fp_lst = [fp_fpr(i)[0] for i in mat_lst]

        sum = 0
        for i in fp_lst:
            sum += i
        
        print('{:.5f}'.format(sum / fp_fpr(mat_lst[0])[1] / len(mat_lst)) + ' ' +  '{:.5f}'.format(sum / len(mat_lst)))

