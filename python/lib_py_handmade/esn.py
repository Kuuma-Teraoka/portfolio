#!/usr/bin/env python3

import numpy as np
from scipy.sparse import csr_matrix

import ndarray
import progress


def esn(w_in, w_x, u, x0): 
    print('esn.model_a.esn(): calculate...',end="", flush=True)
    time = progress.progress()

    w_x = csr_matrix(w_x)

    x = np.ndarray((x0.shape[0], u.shape[1] + 1)) 
    # x0はrandomize_nonzero()のためにn行1列の行列なので、[:,0:1]で行列で切り出して代入。
    x[:, 0:1] = x0

    a = w_in @ u[:, 0]
    b = w_x @ x[:, 0]
    x[:, 1] = np.tanh(np.add(a, b, out=a), out=a) 
    
    for i in range(2, x.shape[1]):
        #x[:, i+1] = np.tanh(np.add(np.dot(w_in, u[:, i], out=a), np.dot(w_x, x[:, i], out=b), out=a), out=a)
        x[:, i] = np.tanh(w_in @ u[:, i-1] + w_x @ x[:, i-1]) 

    print(f'complete in {time.elapsed()}')
    return x


def ridge_regression(d, x, beta): 
    print('esn.model_a.ridge_regression(): calculate...',end="", flush=True)
    time = progress.progress()

    i = ndarray.identity(x.shape[0])
    w_out = d @ x.T @ np.linalg.inv(x @ x.T + beta * i) 
    
    print(f'complete in {time.elapsed()}')
    return w_out

def model_a(beta, w_in, w_x, x0, u, d):
    x = esn(w_in, w_x, u, x0)
    w_out = ridge_regression(d, x[:, 1:], beta)
    y = w_out @ x[:, 1:]
    return x, w_out, y

'''
　初期パラメータであるx(000)を用いて、100の学習データでリザバーを学習させる。そして、そこからフィードバックにより時系列を予測する。この際、この予測に必要なのは状態ベクトルの行列xの最後の状態ベクトルであるx(100)である。これさえあればWoutをかけてy(100)を求めて、y(100)をu(101)として入力する。これにより、y(101)が得られる。これを100回繰り返して、y(200)まで求めればよい。
　よって必要なのは、x(100)と予測時間である100とWout, Win, Wxである。
　ただ、Tvalidを求めたい場合は、100の変わりにdを入力してもいいかもしれない。各時刻(101~200)でy(101)とd(101)、つまりu(102)とベクトルの差の二乗を求めて、その時刻におけるそれが許容値より下の間だけ繰り返し、許容値を超えたらループを抜けるようにする。そして、抜けたらそのときの時刻を出力。いや、分けようかな。
        x(000)
          v
u(001)->x(001)->y(001)
          v
...
          v
u(100)->x(100)->y(100)
          v
y(100)->x(101)->y(101)
          v
...
          v
y(199)->x(200)->y(200)
　というようにすればよい。
'''
def feedback(w_in, w_x, w_out, x100, time):
    print('esn.feedback(): calculate...',end="", flush=True)
    progress.start()

    w_x = csr_matrix(w_x)
 
    x = np.ndarray((x100.shape[0], time + 1)) 
    x[:, 0] = x100 
 
    y = np.ndarray((w_out.shape[0], time + 1)) # y100は含めない。やっぱ含める。
    y[:, 0] = w_out @ x100 

    a = w_in @ y[:, 0]
    b = w_x @ x[:, 0] 
    x[:, 1] = np.tanh(np.add(a, b, out=a), out=a) # y(100)とx(100)からx(101)を求める。。
    y[:, 1] = w_out @ x[:, 1] # x(101)からy(101)を求める。 
    
    for i in range(2, x.shape[1]):
        #np.dot(w_in, u[:, i-1], out=a)
        #np.dot(w_x, x[:, i-1], out=b)
        #x[:, i] = np.tanh(np.add(a, b, out=a), out=a)
        
        x[:, i] = np.tanh(w_in @ y[:, i-1] + w_x @ x[:, i-1])
        y[:, i] = w_out @ x[:, i]
        
    print(f'complete in {progress.elapsed()}')
    return x, y




'''
　y(100)から与えたとき。それがi=0でokとなる。y(140)で許容値を超えたときに、y(100),y(101),...,y(139)がokなのでt_validは40を返したい。y(140)でwhileを抜けるとi=40なので、そのままiを返せばいい。
　y(100)を含めず、y(101)からy(139)の39という値を得たいなら引数にあたえる行列にy(100)を含めなければいい。
　american_institute_of_physics/Hybrid...の式14参照。英語で分母はdenominator, 分子nomeratorというらしいので、そこから変数の名前をdenomとした。
'''
def t_valid(y, d, e=1, debug=False):
    denom = 0
    for i in range(d.shape[1]):
        denom += d[:, i] @ d[:, i]
    denom = (denom / d.shape[1]) ** (1/2)
    print(' ' + str(denom))

    i = 0
    while np.linalg.norm(y[:,i] - d[:,i]) / denom < e:
        if debug: 
            print(f"{i}: {np.linalg.norm(y[:,i] - d[:,i]) / denom} < e:{e}")

        i += 1
        if y.shape[1] <= i or d.shape[1] <= i:
            #raise ValueError("esn.t_valid(): y.shape[1] <= i or d.shape[1] <= i")
            break
        
    if debug:
        print(f"{i}: {np.linalg.norm(y[:,i] - d[:,i]) / denom} >= e:{e}")

    return i




'''
random file usage: 3,881,416 [byte]
　で固定かな。
'''

def parameter(Nu, range_win, Nx, d, spector, rand_file):
    w_in = np.ndarray((Nx, Nu))
    w_in[:, :] = 1
    ndarray.randomize_nonzero(w_in, -range_win, range_win, rand_file)

    x0 = np.ndarray((Nx, 1))
    x0[:, :] = 1
    ndarray.randomize_nonzero(x0, -1, 1, rand_file)

    w_x = ndarray.sparse(Nx, int(Nx * Nx * d), rand_file)
    ndarray.randomize_nonzero(w_x, -1, 1, rand_file)
    print('np.linalg.eig(w_x): calculate...',end="", flush=True)
    time = progress.progress()
    rho = np.max(np.abs(np.linalg.eig(w_x)[0])) 
    #w_x *= spector / rho
    w_x *= rho # 以前はこれだったはず。
    print(f'complete in {time.elapsed()}')

    return w_in, x0, w_x


if __name__ == "__main__":
    
    import sys
    import ndarray

    y = ndarray.read(sys.argv[1])
    d = ndarray.read(sys.argv[2])

    #0-59999が学習。y(100)はy(59999)。lozは20000-79999が入力で20001-80000がラベル。y(101)であるy(60000)に対応するラベルは80001。よってyは60000-69999で、dは80001-90000。ただ89999までなので9999にする。

    # $ print_matrix.py -t -s : 59600:60600 y_0.mat > y.dat
    # $ print_matrix.py -t -s ::3 79601:80601 loz.mat > d.dat
    # $ time_se...

    # $ for ((i=0;i<10;i++)) ; do echo -n "${i}: " ; ~/d/software/handmade/python/lib_py_handmade/esn.py y_${i}.mat loz.mat ; echo -n "${i}: f_value: " ; f_value.py confusion_matrix_${i}.mat ; done

    # 20次元ベクトルの一つの値は-30~30くらいをとる。10ちがうとy-dは(10,10,..)となりすべて2乗して足すと2000になる。30ちがうくらいを閾値にしたいなら900*20で18000
    # 15000だと10000を超えてしまい、14000だといい感じ。
    print(f"t_valid: {t_valid(y[:,60000:], d[:,80001:], e=1)}")


