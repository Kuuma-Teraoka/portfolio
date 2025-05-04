#!/usr/bin/env python3

'''
iterとnextで、for i in mat for j in i、len()はイテラブルなものならなんでもいいのか。ただこれ読み取りしかできない。javaでndarrayのような違うインスタンスだけどdataは同じメモリを参照しているmatrixを作れば書き換え可能。

unsigned int count_matrix(MATRIX *mat, double value) ;
LIST *matrix_to_list(MATRIX *mat) ;
void write_matrix_list_to_dat(FILE *stream, LIST *vector_list) ;
void export_matrix_list_to_dat(LIST *vector_list, char *file_name) ;
LIST *read_matrix_list_from_dat(unsigned int size, FILE *stream) ;
LIST *import_matrix_list_from_dat(char *file_name) ; 

add, sub, mul


'''

import struct
import sys
import numpy as np

import progress
import rand

class Matrix:
    # MATRIX *init_matrix(unsigned int row, unsigned int col) ;
    # void fill_matrix(MATRIX *mat, double a) ;
    # MATRIX *list_to_matrix(LIST *vector_list) ;
    # MATRIX *read_matrix_from_mat(FILE *stream) ;
    # MATRIX *import_matrix_from_mat(char *file_name) ;
    def __init__(self, *args):
        if len(args) == 2 and isinstance(args[0], int) and isinstance(args[1], int):
            self.row = args[0]
            self.col = args[1]
            self.matrix = [0.0] * (self.row * self.col)
        elif len(args) == 3 and isinstance(args[0], int) and isinstance(args[1], int) and isinstance(args[2], float):
            self.row = args[0]
            self.col = args[1]
            self.matrix = [args[2]] * (self.row * self.col)
        elif len(args) == 1 and isinstance(args[0], list): # サブクラスのvector_listに改造
            # vectorのlist(args[0])の1つめのvector(args[0][0])の.matrixの要素数。もし一つ目のvectorがvectorではなくmatrixだったとき、row*colの数が入ってしまうが、このあとのifでraiseされる。
            self.row = len(args[0][0].matrix)
            self.col = len(args[0])
            self.matrix = [0.0] * (self.row * self.col)
            for i, mat in enumerate(args[0]):
                if mat.row != self.row or mat.col != 1:
                    raise ValueError("mat.row != self.row or mat.col != 1")
                for j in range(self.row):
                    self[j][i] = mat[j][0]
        elif len(args) == 1 and isinstance(args[0], str) and args[0].endswith(".mat"):
            with open(args[0], "rb") as f:
                self.row, self.col = struct.unpack("II", f.read(8))
                tuple_matrix = struct.unpack(f"{self.row * self.col}d", f.read(8 * self.row * self.col))
                self.matrix = list(tuple_matrix)
        # ndarrayの変換。tolistでlistにしてからMatrixにする。各行の列数が同じであることを前提として、self.colを0行目の列を参照する。
        elif len(args) == 1 and isinstance(args[0], np.ndarray):
            mat_list = args[0].tolist()
            self.row = len(mat_list)
            self.col = len(mat_list[0])
            self.matrix = []
            for i in mat_list:
                self.matrix += i
        else:
            raise TypeError("else")

    # double get_matrix(MATRIX *mat, unsigned int row, unsigned int col) ;
    # とりあえずmat[3]としたときに、3行目のlistを返すようにする。さすれば、listには__getitem__が定義されているので、mat[3][4]で3行4列が取り出せる。5列なら10:15を返せばいい。
    # [3,4]とするとidxにtupleが与えられる。tupleに対応させた。
    def __getitem__(self, idx):
        if isinstance(idx, tuple) and len(idx) == 2:
            return self.matrix[idx[0] * self.col + idx[1]]
        elif isinstance(idx, int):
            return self.matrix[self.col * idx : self.col * (idx + 1)]
        else:
            raise TypeError("not isinstance(idx, tuple) and not isinstance(idx, int), type: " + str(idx) + str(type(idx)))
    # 旧型
    def get_value(self, row, col):
        return self.matrix[row * self.col + col]
    
    # void set_matrix(MATRIX *mat, unsigned int row, unsigned int col, double value) ;
    # __setitem__が定義されていなくてもmat[3][4]=4で値を入れることができる。これはlistに__setitem__が定義されているからである。どういうことかというと、__getitem__を定義したので、mat[3]が3行目のリストを参照できる。よって、mat[3][4]は(3行目のリスト)[4]という意味になるので、mat[3][4]=4で使われるのはlistの__setitem__である。
    # それでも行をまとめて入れる、つまり、mat[3] = [1,2,3]とすることがあるかもしれないので定義。
    def __setitem__(self, idx, value):
        if isinstance(idx, tuple) and isinstance(value, float):
            self.matrix[idx[0] * self.col + idx[1]] = value
        elif isinstance(idx, int) and isinstance(value, list) and len(value) == self.col:
            for i, element in enumerate(value):
                if not isinstance(element, float):
                    raise TypeError("isinstance(element, float)")
                self.matrix[self.col * idx + i] = element
        else:
            raise TypeError("else")
    # 旧型
    def set_value(self, row, col, value):
        self.matrix[row * self.col + col] = value

    # void write_matrix_to_dat(MATRIX *mat, FILE *stream) ;
    # void write_vector_to_dat(MATRIX *vec, FILE *stream) ;
    # col==1のときに1 2 1 ...というようにベクトルのように出力するようにする。chatGPTのコードはlistを返す
    # str.join()に与えるリストの各要素はstrでないとTypeError: sequence item 0: expected str instance, float foundというエラーが出るので、すべてstrに変換。この際、Matrixのmatrixという名前のlistはすべて不変オブジェクトで構成されているとする。これが制約。
    # まず__getitem__により、mat[i]で行のlistを取り出す。copyでメモリにコピーし、すべてstrに変換。あとはそのstrのlistをjoinに渡して空白区切りの長いstrを作り、それを各行分つくりresultというlistに入れていく。あとは、\nのjoinにresultを与えれば終わり。
    # strのリストを作る際に、すべてコピーして書き換える方法と、空のリストに、すべての要素に対して、strに変換したものを加えていく方法の二つがある。後者は[for]というリテラル表記とforを組み合わせた文法が使えるのでそれを使うようにした。
    def __str__(self):
        if self.col == 1:
            return ' '.join([str(value) for value in self.matrix])
        else:        
            result = []
            for i in range(self.row):
                result.append(' '.join([str(value) for value in self[i]]))
            return '\n'.join(result)
    
    def __repr__(self):
        return self.__str__()

    def __add__(self, other):
        if isinstance(other, type(self)):
            result = type(self)(self.row, self.col)
            for i in range(self.row):
                for j in range(self.col):
                    result[i, j] = self[i, j] + other[i, j]
            return result
        else:
            return NotImplemented
    
    def __sub__(self, other):
        if isinstance(other, type(self)):
            result = type(self)(self.row, self.col)
            for i in range(self.row):
                for j in range(self.col):
                    result[i, j] = self[i, j] - other[i, j]
            return result
        else:
            return NotImplemented

    # mul
    # void scale_matrix(double c, MATRIX *a) ;
    # progressを演算数が一定値以上だったときに表示。progress_fragは演算子*では使えないので。progress_fragをprogress.pyのグローバル変数にしてもいいかも。デフォルトではTrueで、演算数が少ないときは一時的にFalseにすることでオフにして、最後にTrueに戻す。sys.stdoutを変えるのと同じ。
    def __mul__(self, other):
        if isinstance(other, type(self)):
            if self.col != other.row:
                raise ValueError("self.col != self.row")
            progress_frag = False
            if self.row * other.col * self.col > 100000000000:
                progress_frag = True
            result = type(self)(self.row, other.col)
            for i in range(result.row):
                for j in range(result.col):
                    temp = 0
                    for k in range(self.col):
                        temp += self[i, k] * other[k, j]
                    result[i, j] = temp
                if progress_frag:
                    progress.update(i, result.row)
            return result
        elif isinstance(other, int) or isinstance(other, float):
            result = type(self)(self.row, self.col)
            for i in range(self.row):
                for j in range(self.col):
                    result[i, j] = other * self[i, j]
            return result
        else:
            return NotImplemented
    
    def __rmul__(self, other):
        # __mul__が使われるので必要ない。
        # if isinstance(other, type(self)):
        # 3 * matrixという場合。intやfloatにはmatrixとの演算は設定されておらずNotImplementedを返すので、こっちで扱える。
        if isinstance(other, int) or isinstance(other, float):
            return self.__mul__(other)
        else:
            raise TypeError("result == NotImplemented:")

    # void write_matrix_to_mat(MATRIX *mat, FILE *stream) ;
    # void export_matrix_to_mat(MATRIX *mat, char *file_name) ;
    # void export_matrix_to_dat(MATRIX *mat, char *file_name) ;
    # MATRIX *import_matrix_from_dat(char *file_name) ;
    def export(self, file_name):
        if isinstance(file_name, str) and file_name.endswith(".mat"):
            with open(file_name, 'xb') as f:
                f.write(struct.pack('II', self.row, self.col))
                f.write(struct.pack(f"{self.row * self.col}d", *self.matrix))
            print("generated", file_name)
        elif isinstance(file_name, str) and file_name.endswith(".dat"):
            with open(file_name, 'xb') as f:
                f.write((str(self) + '\n').encode('utf-8'))
            print("generated", file_name)
        else:
            raise TypeError("else:")
        
    def T(self):
        result = type(self)(self.col, self.row)
        for i in range(result.row):
            for j in range(result.col):
                result[i, j] = self[j, i]
        return result
        
    def ndarray(self):
        return np.array([self[i] for i in range(self.row)])
    
    # void randomize_matrix_nonzero(FILE *rand_file, MATRIX *mat, double from, double to) ;
    def randomize_nonzero(self, rand_file, a, b):
        for i in range(self.row):
            for j in range(self.col):
                if self[i, j] != 0:
                    self[i, j] = rand.fcustom(rand_file, a, b)

    
    # MATRIX *generate_sparse_matrix(FILE *rand_file, unsigned int row, unsigned int col, double p) ;
    @classmethod
    def sparse(cls, row: int, col: int, p: float, rand_file):
        mat = cls(row, col)

        for i in range(row):
            for j in range(col):
                if rand.f01(rand_file) < p:
                    mat[i, j] = 1.0
        return mat
    
    def __iter__(self):
        return MatrixIterator(self)
class MatrixIterator:
    def __init__(self, mat):
        if isinstance(mat, Matrix):
            self.rows = mat.row
            self.cols = mat.col
            self.matrix = mat.matrix
            self.index = 0
        else:
            raise TypeError("else")
    # def __iter__(self):
    #     return self
    def __next__(self):
        if self.index >= self.rows * self.cols:
            raise StopIteration
        value = self.matrix[self.index]
        self.index += 1
        return value



if __name__ == "__main__":

    import sys
    
    a = Matrix(500,1000)
    b = Matrix(1000,400)
    # a = Matrix(10000,10000, 1.0)
    # a[1,1] = 3.0
    # b = Matrix(10000,2000, 1.0)

    with open("/home/lucifer/mybin/randfile_s2_1g.bin", "rb") as rand_file:
        a.randomize_nonzero(rand_file, -1.0, 1.0)
        b.randomize_nonzero(rand_file, -1.0, 1.0)

    progress.update(0,2)
    c = a * b
    progress.update(1,2)
    
    progress.update(0,2)
    d = Matrix(a.ndarray() @ b.ndarray())
    progress.update(1,2)

    progress.total()
    progress.peak()

    '''
    　mul_speed.cと比較。
    500*1000 matrix * 1000*400 matrix, estimate: 500*400*1000=200000000
    　で検証。
    mul_speed.c: calcurate time of mul: 0.198313 [sec]
    matrix.py: calcurate time of mul: 83.41240499701235 [sec]
    　これはえぐい。3倍程度ならまだしも、420.60986923203393倍。

    　numpyの@と比較。変換も含めても早い場合にnumpyに変換してmatに戻すようにする。
    $ python python/py_library_handmade/matrix.py 100
    calcurate time of me: 0.00434179999865591526 [sec]
    calcurate time of np: 0.00018350000027567148 [sec]
    $ python python/py_library_handmade/matrix.py 15
    calcurate time of me: 0.00066860002698376775 [sec]
    calcurate time of np: 0.00009569997200742364 [sec]
    $ python python/py_library_handmade/matrix.py 11
    calcurate time of me: 0.00051189999794587493 [sec]
    calcurate time of np: 0.00039050000486895442 [sec]
    $ python python/py_library_handmade/matrix.py 10
    calcurate time of me: 0.00048439996317028999 [sec]
    calcurate time of np: 0.00047220004489645362 [sec]
    $ python python/py_library_handmade/matrix.py 9
    calcurate time of me: 0.00045220000902190804 [sec]
    calcurate time of np: 0.00036790000740438700 [sec]
    $ python python/py_library_handmade/matrix.py 8
    calcurate time of me: 0.00037909997627139091 [sec]
    calcurate time of np: 0.00008520000847056508 [sec]
    $ python python/py_library_handmade/matrix.py 7
    calcurate time of me: 0.00034540001070126891 [sec]
    calcurate time of np: 0.00008560001151636243 [sec]
    $ python python/py_library_handmade/matrix.py 6
    calcurate time of me: 0.00030409998726099730 [sec]
    calcurate time of np: 0.00038899999344721437 [sec]
    　これはsys.argv[1] * 10 matrix @ 10 * 10 matrixの計算結果。row * col > 1000でいいかな。
    '''
