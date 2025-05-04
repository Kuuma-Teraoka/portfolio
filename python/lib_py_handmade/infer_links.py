#!/usr/bin/env python3

'''
　60*60*60000で1:26。np.abs()にすることで、pythonループを削除する前は2:30くらいだった。cppより早かったかもしれない。唯一対角成分だけ-1するときのpythoループが残っていたので、ideを生成しておく方法にしたら、1:05。まじか。pythonループを除けば、生成のコードでも、cppの書き換えより早い。インスタンスを生成している以上、遅いはずなのに、これはnumpyの最適化なのか、pythonの最適化なのか。いや、平均1:13くらいだからそこまでかも。
'''

import numpy as np

import ndarray
import progress

def stcd(tau, w_in, w_x, w_out, u):
    x = w_x @ np.linalg.pinv(w_out) + w_in

    sum = np.ndarray((u.shape[0], u.shape[0])) 
    sum[:, :] = 0.0
    ide = ndarray.identity(sum.shape[0])
    bar = progress.progress(u.shape[1], 'stcd():')
    for i in range(u.shape[1]):
        z = u[:, i].reshape(u.shape[0], 1)
        #dhdt = w_out @ (ndarray.derivative_tanh(x @ z) * x)
        dhdt = (w_out * (ndarray.derivative_tanh(x @ z)).T) @ x
        dhdt -= ide
        '''for j in range(dhdt.shape[0]):
            dhdt[j,j] = dhdt[j,j] - 1'''
        dhdt /= tau
        dhdt = np.abs(dhdt)
        '''for j in range(dhdt.shape[0]):
            for k in range(dhdt.shape[1]):
                if dhdt[j,k] < 0:
                    dhdt[j,k] = -1 * dhdt[j,k]'''
        sum += dhdt
        bar.update(i)
    
    return sum / u.shape[1]



def bayesian(edge, fkl):
    result = np.ndarray(fkl.shape)
    result[:,:] = 0

    for k in range(edge):
        max = -1
        i_max = 0
        j_max = 0

        for i in range(fkl.shape[0]):
            for j in range(fkl.shape[1]):
                if i != j and result[i, j] == 0 and fkl[i, j] > max:
                    max = fkl[i, j]
                    i_max = i
                    j_max = j
        result[i_max, j_max] = 1

    return result





'''
TN -> 0
TP -> 1
FN -> 2
FP -> 3
　となっている。メモはinfer_links.hpp参照。
'''
def confusion_matrix(truth_links, inferred_links):

    result = np.ndarray(truth_links.shape)
    for i in range(result.shape[0]):
        for j in range(result.shape[1]):
            if truth_links[i, j] == 0 and inferred_links[i, j] == 0:
                result[i, j] = 0
            elif truth_links[i, j] == 1 and inferred_links[i, j] == 1:
                result[i, j] = 1
            elif truth_links[i, j] == 1 and inferred_links[i, j] == 0:
                result[i, j] = 2
            elif truth_links[i, j] == 0 and inferred_links[i, j] == 1:
                result[i, j] = 3
    return result





'''
　時間に対して値を一つもつ1次元の時系列を受け取って、自己相関が最初に0になる時差tauを返す関数。詳しい説明はtexにまとめたほうがいいと思う。
　lozはxだけだとしても20次元時系列だが、それぞれにtauを求めるのだろうか。それとも、高次元ベクトルの配列に対しても自己相関が求まるのか聞く。いや、どうやらそれぞれ求めて平均するなどする必要があるらしい。
'''
def autocorrelation(ts): 
    tau = 0
    while np.corrcoef(np.stack((ts[:ts.shape[0]-tau], ts[tau:]), axis=0))[0,1] > 0 :
        #print(np.corrcoef(np.stack((ts[:ts.shape[0]-tau], ts[tau:]), axis=0))[0,1])
        tau += 1
    return tau


if __name__ == "__main__":
    import os
    import lorenz
    import ndarray
    with open(f"{os.environ['RINDFILE_PATH']}/1.bin", "rb") as rand_file:
        truth_links = ndarray.sparse(5, 10, rand_file) 
        loz = lorenz.lorenz(truth_links, 0.06, 0.02, 6000, rand_file)
    for i in range(loz.shape[0]):
        print(autocorrelation(loz[i,:]))


 





