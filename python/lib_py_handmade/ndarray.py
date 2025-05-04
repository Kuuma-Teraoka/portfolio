#!/usr/bin/env python3

'''
　numpy用の自作関数。ndarrayを継承しても、numpyは関数志向であるゆえ、ndarrayのインスタンスmethod以外のほとんどの関数がつかえなくなってしまうはず。検証はしていないが。
　本来ならndarrayのインスタンスmethodとして追加したいが、しかたなく、method(ndarray, a, b)という関数をたくさんつくる。
'''

import numpy as np
import struct

import progress
import rand
import strings

#class ndarray :


def write(mat, file_name):
    if isinstance(mat, np.ndarray) and isinstance(file_name, str) and file_name.endswith(".mat"):
        with open(file_name, 'xb') as f:
            f.write(struct.pack('II', mat.shape[0], mat.shape[1]))
            #f.write(struct.pack(f'{mat.shape[0] * mat.shape[1]}d', *mat.flatten()))
            f.write(mat.tobytes())
        print("generated", file_name)
    else:
        raise TypeError("else")

# importだとimport文になってしまう。予約語。
def read(file_name):
    with open(file_name, 'rb') as f:
        rows, cols = struct.unpack('II', f.read(8))
        mat = np.fromfile(f, dtype=np.float64)
        mat = mat.reshape((rows, cols))
    return mat

def __str__(mat):
    if isinstance(mat, np.ndarray):
        result = []
        for i in mat:
            result.append(' '.join([str(value) for value in i]))
        return '\n'.join(result)





def count(mat: np.ndarray, value: float):
    count = 0
    for i in mat.reshape(mat.shape[0] * mat.shape[1]):
        if i == value:
            count += 1
    return count




def mul_progress(*args):
    if len(args) == 3:
        ar, ac = args[0].shape
        br, bc = args[1].shape
        print(f'({ar},{ac})matrix @ ({br},{bc})matrix (estimate: {ar}*{bc}*{ac} = {strings.comma(str(ar * bc * ac))}): \n\t', end="")
        progress.update(0,2)
        np.dot(args[0], args[1], out=args[2])
        progress.update(1,2)
        return args[2]
    elif len(args) == 2:
        result = np.ndarray((args[0].shape[0], args[1].shape[1]))
        return mul_progress(*args, result)
    else:
        raise TypeError("else")

def mul(*args):
    if len(args) == 3:
        ar, ac = args[0].shape
        br, bc = args[1].shape
        print(f'({ar},{ac})matrix @ ({br},{bc})matrix (estimate: {ar}*{bc}*{ac} = {strings.comma(str(ar * bc * ac))}): \n\t', end="")
        max = 10
        for i in range(max):
            start = int(args[0].shape[0] * i / max)
            end = int(args[0].shape[0] * (i + 1) / max)
            args[2][start:end, :] = np.dot(args[0][start:end, :], args[1])
            progress.update(i, max)
        return args[2]
    elif len(args) == 2:
        result = np.ndarray((args[0].shape[0], args[1].shape[1]))
        return mul(*args, result)
    else:
        raise TypeError("else")

# np.tanh()がndarrayの各要素に対し求めてくれるのでそれの拡張    
def derivative_tanh(x):
    return 1 - np.tanh(x) ** 2

# nodes * nodesの行列を作り0で初期化、edgesの数だけランダムに1にして返す関数。aklとW_xの生成に使える。自己ループなし。
def sparse(nodes, edges, rand_file):
    if isinstance(nodes, int) and isinstance(edges, int):
        if edges > nodes * nodes - nodes:
            raise ValueError("edges > nodes * nodes - nodes")
        result = np.ndarray((nodes, nodes))
        result.fill(0.0)
        i = 0
        while(i < edges):
            rand_value = rand.lu(rand_file) % (nodes * nodes)
            row = rand_value // nodes
            col = rand_value % nodes
            if result[row, col] == 0 and row != col:
                result[row, col] = 1
                i += 1
        return result
    else:
        raise TypeError("else")

def randomize_nonzero(mat, start, end, rand_file):
    progress_frag = False
    if mat.shape[0] * mat.shape[1] > 5000 * 5000: #5000*5000で10sくらい
        progress_frag = True
    
    if progress_frag:
        print(f"randomize_nonzero(({mat.shape[0]},{mat.shape[1]})matrix):\n\t", end="")
    for i in range(mat.shape[0]):
        if progress_frag:
            progress.update(i, mat.shape[0])
        for j in range(mat.shape[1]):
            if mat[i, j] != 0:
                mat[i, j] = rand.fcustom(rand_file, start, end)
    
def identity(demention):
    result = np.ndarray((demention, demention))
    result.fill(0)
    for i in range(demention):
        result[i, i] = 1
    return result


def norm(vec):
    if len(vec.shape) != 1:
        raise TypeError("len(vec.shape) != 1")
    return (vec @ vec) ** (1 / 2)

# ベクトルの平均。ベクトルは所詮複数の値を集めたものなので、それぞれの値に対して平均を求めればいい。一次ndarrayに対して平均を求めるものを作ってから、行列の各行をそれに与えようと思ったが、ベクトルの足し算や割り算がちゃんと定義されているのでそれを使うことにした。1次元のほうはnp.float64を返していることに注意。
def ave(mat):
    if isinstance(mat, np.ndarray) and len(mat.shape) == 1:
        sum = 0
        for i in mat:
            sum += i
        return sum / mat.shape[0]
    elif isinstance(mat, np.ndarray) and len(mat.shape) == 2:
        sum = 0
        for i in range(mat.shape[0]):
            sum += mat[:,i]
        return sum / mat.shape[0]
    else:
        raise TypeError("else")

# dの分散。3.7.1参照。
def var(mat_d):
    ave_d = ave(mat_d)
    sum = 0
    for i in range(mat_d.shape[1]):
        sum += norm(mat_d[:, i] - ave_d) ** 2
    return (sum / mat_d.shape[1]) ** (1 / 2)

# 60*60の行列と、(3*3)というtupleを受け取って、それぞれの3*3の行列を引数のmethodに渡して、スカラーに変換し、20*20の行列を返す関数。
def block(mat, block_shape, method):
    if mat.shape[0] % block_shape[0] != 0 or mat.shape[1] % block_shape[1] != 0:
        raise ValueError("mat.shape[0] % block_shape[0] != 0 or mat.shape[1] % block_shape[1] != 0")

    result = np.ndarray((mat.shape[0] // block_shape[0], mat.shape[1] // block_shape[1]))
    mat = mat.reshape((result.shape[0], block_shape[0], result.shape[1], block_shape[1]))

    for i in range(result.shape[0]):
        for j in range(result.shape[1]):
            result[i, j] = method(mat[i, :, j, :])

    return result

# 対角成分をdiagonal elementというので。
def diagonal(mat):
    if mat.shape[0] != mat.shape[1]:
        raise ValueError("mat.shape[0] != mat.shape[1]")
    sum = 0
    for i in range(mat.shape[0]):
        sum += mat[i,i]
    return sum

        
    


if __name__ == "__main__":
     
    #with open(sys.argv[1], "rb") as rand_file:
        #a = np.ndarray((10000,20000))
        #b = np.ndarray((20000,3000))
        #a.fill(2.34)
        #b.fill(1)
        # randomize_nonzero(b, -1., 1., rand_file)

    
    #d = mul_progress(a, b)
    #c = mul(a, b)
    #print(d == c)
    #print(np.allclose(d, c))

    #a = np.array([[3,3,3],[2,2,1],[0,6,8]])
    #print(count(a, 0.))
    #print(count(a, 3.))

    np.ndarray((5000, 60000)) @ np.ndarray((60000,5000))
    #print(a)
    #print(diagonal(a))
    #print(block(a,(2,2),diagonal))







    progress.total()
    progress.peak()

'''
(10000,20000)matrix @ (20000,3000)matrix (estimate: 10000*3000*20000 = 600000000000): 
        <----------10%----------20%----------30%----------40%-------------------60%----------70%----------80%----------90%---------> complete in 5.465232715010643 [sec]
(10000,20000)matrix @ (20000,3000)matrix (estimate: 10000*3000*20000 = 600000000000): 
        <----------10%----------20%----------30%----------40%----------50%----------60%----------70%----------80%----------90%----------100%> complete in 4.0:14.858348571986426 [sec]
total calculation time: 4:20.756222486990737 [sec]
　10000刻み。5秒と4分の差なのでめちゃくちゃでかい。

(10000,20000)matrix @ (20000,3000)matrix (estimate: 10000*3000*20000 = 600000000000): 
        <----------10%----------20%----------30%----------40%-------------------60%----------70%----------80%----------90%---------> complete in 4.827135311003076 [sec]
(10000,20000)matrix @ (20000,3000)matrix (estimate: 10000*3000*20000 = 600000000000): 
        <----------10%----------20%----------30%----------40%----------50%----------60%----------70%----------80%----------90%----------100%> complete in 9.010523567994824 [sec]
total calculation time: 14.317558580980403 [sec]
　100刻み。1.8666399401433895遅い。

　詳しくはコード。いや、以下にコードコピペ。進捗表示したいなら10000個に分けずとも、100個にわけて積をもとめればいい。
for i in range(100):
    start = int(args[0].shape[0] * i / 100)
    end = int(args[0].shape[0] * (i + 1) / 100)
    args[2][start:end, :] = np.dot(args[0][start:end, :], args[1])
    progress.update(i, 100)
　こんな感じ

(10000,20000)matrix @ (20000,3000)matrix (estimate: 10000*3000*20000 = 600000000000): 
        <----------10%----------20%----------30%----------40%----------50%----------60%----------70%----------80%----------90%----------100%> complete in 4.810663164011203 [sec]
(10000,20000)matrix @ (20000,3000)matrix (estimate: 10000*3000*20000 = 600000000000): 
        <----------10%----------20%----------30%----------40%----------50%----------60%----------70%----------80%----------90%----------100%> complete in 6.786646407010267 [sec]
total calculation time: 12.80329686999903 [sec]
　20刻み。つまり5%ずつ進行。1.4107506960332388遅い。

(10000,20000)matrix @ (20000,3000)matrix (estimate: 10000*3000*20000 = 600000000000): 
        <----------10%----------20%----------30%----------40%----------50%----------60%----------70%----------80%----------90%----------100%> complete in 5.862995312985731 [sec]
(10000,20000)matrix @ (20000,3000)matrix (estimate: 10000*3000*20000 = 600000000000): 
        <----------10%----------20%----------30%----------40%----------50%----------60%----------70%----------80%----------90%----------100%> complete in 4.474294210987864 [sec]
total calculation time: 11.196341827017022 [sec]
　10刻み。なぜか早くなった。え、なんで。
'''
    
