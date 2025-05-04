#!/usr/bin/env python3

'''
　dat形式ではなく、.mat形式で時系列データを作る。datでテキストにしたときにデータをロストする可能性がある。まあ、実際にcmpで調べないとわからないが。といっても、プログラム内ではfloat64で扱っているわけで、float64でexportするほうが自然だと思う。見たいときだけ、.matを.datに変換してみればいい。
　ベクトルは列ベクトルとして、横に結合する。
　入力用の0~tのloz_0.matと、学習用の1~t+1のloz_1.matを生成するようにした。

　t+10000まで生成して、保存しておいて、あとで時系列予測させて学習できているかを調べられるようにする。あと、hk.matもちがうスクリプトで生成して、それを読み込むようにする。

　ファイルに書き込むうえでは、列優先形式のほうがいいので、列優先でexportする。といっても、ベクトルを毎回SSDに書き込むのと、メモリで行列をつくってからまとめてSSDに書き込むのとでどちらがいいのかはわからない。メモリ使用量と計算時間を最後に出して検証する。

random file usage: 160 [byte]
　で固定。20次元ベクトルhをrandomizeするときだけ。20はaklで決まる

　さて、全て改装する。まず、関数にして、example_1.shをexample_1.pyに。

　20241223現在、60*90000で47s。あれ、まえは3mくらいで、0.02を0.001にすると60mだったはず。progressでstep1000にしたからなのかな。
'''

import numpy as np

import ndarray
import progress
import os
rand_dir = os.environ['RINDFILE_PATH']


def differential_equations(vec_m, akl, h_vec, c):
    # vec_m.shape[0] % 3 == 0はrunge_kutta()で確認済み。
    derivative_vec = vec_m.copy()

    sigma = 10
    rho = 28
    beta = 8 / 3
    dyn = 0

    for i in range(vec_m.shape[0] // 3):
        x = vec_m[3 * i + 0]
        y = vec_m[3 * i + 1]
        z = vec_m[3 * i + 2]
        sum = 0
        for j in range(akl.shape[1]):
            sum += akl[i, j] * (vec_m[3 * j + 1] - y)
        derivative_vec[3 * i + 0] = -sigma * (x - y + c * sum) + dyn
        derivative_vec[3 * i + 1] = rho * (1 + h_vec[i, 0]) * x - y - x * z + dyn
        derivative_vec[3 * i + 2] = -beta * z + x * y + dyn
    
    return derivative_vec



'''
　C言語では列ベクトルを受け取るようにしていた。しかし、ndarrayでは、__getitem__でsliceで列ベクトルを取り出すと1次元行ベクトルになるのと、__setitem__でsliceで上書きするときも1次元行ベクトルを受け取って、列を上書きする仕様になっている。よって、これらの関数では行ベクトルを扱うように変えた。
　といっても、2次元の場合はrow:shape[0], col:shape[1]だが、1次元の場合はshape[0]がそのまま要素数になり、shape[1]はそもそもメモリにないので、2次元列ベクトルの要素数はrow:shape[0]だし、1次元行ベクトルの要素数もshape[0]であることに注意。
'''
def runge_kutta(vec_m, akl, h_vec, c, tau):
    if not isinstance(vec_m, np.ndarray) or not isinstance(akl, np.ndarray) or not isinstance(h_vec, np.ndarray):
        raise TypeError("not isinstance(vec_m, np.ndarray) or not isinstance(akl, np.ndarray) or not isinstance(h_vec, np.ndarray)")
    if vec_m.shape[0] % 3 != 0:
        raise ValueError("vec_m.shape[0] % 3 != 0")

    k1 = tau * differential_equations(vec_m, akl, h_vec, c)
    k2 = tau * differential_equations(vec_m + 1 / 2 * k1, akl, h_vec, c)
    k3 = tau * differential_equations(vec_m + 1 / 2 * k2, akl, h_vec, c)
    k4 = tau * differential_equations(vec_m + k3, akl, h_vec, c)
    next_vec_m = vec_m + 1 / 6 * (k1 + 2 * k2 + 2 * k3 + k4)

    return next_vec_m





def time_series(akl:np.ndarray, h_value:float=0.06, c:float=0.3, tau:float=0.02, time:int=60000, rand_file=f"{rand_dir}/1.bin"):  

    # randomize_nonzeroがshape[0], shape[1]でfor文を構成しているので、(n)でなく、(n,1)にする。
    h_vec = np.ndarray((akl.shape[0], 1))
    h_vec[:, :] = 1
    ndarray.randomize_nonzero(h_vec, -h_value, h_value, rand_file)
    
    result = np.ndarray((3 * akl.shape[0], time))
    result[:, 0] = 1 
    ndarray.randomize_nonzero(result[:, 0:1], -1, 1, rand_file) 

    bar = progress.progress(time, "coupled_lorenz():")
    for i in range(1, time) : 
        # result[:, i-1]は1次元ndarray。copyしているので、runge_kuttaが返すのも1次元。result[:, i]も1次元なので代入できる。
        result[:, i] = runge_kutta(result[:, i - 1], akl, h_vec, c, tau)
        bar.update(i)
    
    # 1,1,1,...,1を、1, 19,1, 19,1, ..., 19,1にする。
    #tmp = result[:, 0]
    #for i in range((time-1)*20) : 
    #    tmp = runge_kutta(tmp, akl, h_vec, c, tau / 20)
    #    if (i+1) % 20 == 0:
    #        result[:, (i+1) // 20] = tmp
    #    progress.update(i, (time-1)*20, step=1000, label='lorenz():')



    return result

