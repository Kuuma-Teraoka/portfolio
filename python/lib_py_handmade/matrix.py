#!/usr/bin/env python3


'''
　cmat, fmatはnumpy.txt参照。
'''

import numpy as np
import struct
from collections.abc import Iterable

import progress
import rand
import strings

class matrix(np.ndarray):

    # python.txtのサブクラスにかいたようにnumpyは__new__が最初によばれるので、こっちで定義する。"a.cmat"といったファイル名に対してはread処理、numpy.txtに書いているようにndarray(3,3)はエラーで、ndarray((3,3))とする必要があるので、(3,3)というようにintだけならtupleにまとめてndarrayに渡す。あと、ndarrayに(3,3)を与えると3行3列だが、((3,3),(3,3))だとエラーがでる。なので、このような2次元配列以上の場合にnp.arrayを呼ぶようにする。
    def __new__(cls, *args):
        if len(args) == 1 and isinstance(args[0], str): 
            if args[0].endswith(".fmat"):
                order='F'
            elif args[0].endswith(".cmat"):
                order='C'
            else:
                raise TypeError("matrix.__new__(): file not fmat or cmat.")
            with open(args[0], 'rb') as f:
                rows, cols = struct.unpack('II', f.read(8)) 
                return np.fromfile(f,dtype=np.float64).reshape((rows,cols), order=order).view(cls)
        elif all(isinstance(i, int) for i in args):
            return super().__new__(cls, args)
        elif len(args) == 1 and isinstance(args[0], Iterable) and all(isinstance(i, Iterable) for i in args[0]):
            return np.array(args[0]).view(cls)
        else:
            return super().__new__(cls, *args)

    def write(self, file_name: str): 
        if file_name.endswith(".cmat"):
            order='C'
        elif file_name.endswith(".fmat"):
            order='F'
        else:
            raise TypeError("matrix.write(): file not fmat or cmat.")
        with open(file_name, 'xb') as f:
            f.write(struct.pack('II', self.shape[0], self.shape[1])) 
            f.write(self.tobytes(order=order))
        print("generated", file_name)

    def __str__(self): 
        result = []
        for i in self:
            result.append(' '.join([str(value) for value in i]))
        return '\n'.join(result)

    def __repr__(self):
        return f"{a.header()}\n{a}"

    def header(self):
        return f"ndim: {self.ndim}, shape: {self.shape}, strides: {self.strides}, dtype: {self.dtype}"






    def count(self, value: float):
        count = 0
        for i in self.reshape(self.shape[0] * self.shape[1]):
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
    # read, write()
    a = matrix(((1.,8,8),(1,1,1)))
    print(a.__repr__())
    a.write("a.cmat")
    a.write("a.fmat")
    c = matrix("a.cmat")
    f = matrix("a.fmat")
    print(c.__repr__())
    print(f.__repr__())

    #
   
