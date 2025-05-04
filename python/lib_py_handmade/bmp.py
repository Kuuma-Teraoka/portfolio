#!/usr/bin/env python3

'''
byte_rowcol = struct.pack('II', self.row, self.col)
byte_mat = str_mat.encode('utf-8')
self.row, self.col = struct.unpack("II", byte_rowcol)
str_mat = byte_mat.decode('utf-8')

　所詮はRGBの行列。デフォルトをアルファチャンネルありの4byte、つまり32bitににする。

bmp.bmp(3, 3, "FFA500").export("orange.bmp")
　で3*3にFFA500にアルファチャンネルのFFを加えたもので埋めたbmpを生成できるようにしたい。

　matrix.pyのMatrixを継承して、export、import(__init__)だけ上書きする。__getitem__とかはそのまま使えるはず。ただ、list matrixにrgbクラスをいれるだけ。
　__str__もそのままつかえてlist matrixのそれぞれをそのままstr()に与えたものを空白区切りのstrに変換するだけなので、ちゃんとFFA500 FFA500とrgb.__str__()を使ってくれた。


'''

import struct
from matrix import Matrix

class bmp(Matrix):
    def __init__(self, *args):
        # bmp(3, 3, "FFA500"0
        if len(args) == 3 and isinstance(args[0], int) and isinstance(args[1], int) and isinstance(args[2], str):
          self.row = args[0]
          self.col = args[1]
          self.matrix = [rgb(args[2])] * (self.row * self.col)
        else:
            raise TypeError("else")

    def export(self, file_name):
        if isinstance(file_name, str) and file_name.endswith(".bmp"):
            with open(file_name, 'xb') as f:
                file_header = 14
                dib_header = 40
                image_size = 4 * self.col * self.row
                f.write(
                    "BM".encode('utf-8') +
                    (file_header + dib_header + image_size).to_bytes(4, 'little') +
                    (0).to_bytes(2, 'little') +
                    (0).to_bytes(2, 'little') +
                    (file_header + dib_header).to_bytes(4, 'little')
                )
                f.write(
                    (dib_header).to_bytes(4, 'little') +
                    self.col.to_bytes(4, 'little') +
                    self.row.to_bytes(4, 'little') +
                    (1).to_bytes(2, 'little') +
                    (32).to_bytes(2, 'little') + # alphaありの32bitを前提に。
                    (0).to_bytes(4, 'little') +
                    (image_size).to_bytes(4, 'little') +
                    (0).to_bytes(4, 'little') + # dpiなんてしらない
                    (0).to_bytes(4, 'little') + # dpiなんてしらない
                    (0).to_bytes(4, 'little') +
                    (0).to_bytes(4, 'little')
                )
                for i in range(self.row):
                    for j in range(self.col):
                        f.write(self[i, j].to_bytes())
            print("generated", file_name) 
        else:
            raise TypeError("else:")        

 #   def __init__(self, *args):
  #      if len(args) ==  

class rgb:
    def __init__(self, *args):
        # rgb("FFA500")
        if len(args) == 1 and isinstance(args[0], str):
            self.r = int(args[0][0:2], 16)
            self.g = int(args[0][2:4], 16)
            self.b = int(args[0][4:6], 16)
            self.alpha = 0xFF
        else:
           raise TypeError("else")

# FFA500というような16進数表記がなんだかんだ一番わかりやすいので、16進数表記のstrに変換する。
# alphaがFF意外、つまり不透明でないときだけ、alphaを加えて"FFA50099"というようなstrに変換する。
    def __str__(self):
        if self.alpha == 0xFF:
            return type(self).hex(self.r) + type(self).hex(self.g) + type(self).hex(self.b)
        else:
            return type(self).hex(self.r) + type(self).hex(self.g) + type(self).hex(self.b) + type(self).hex(self.alpha) 

    # intを受け取って、"A5"のような16進数2文字のstrを返す関数。0未満FFより大きい値はエラー。
    def hex(num):
        if isinstance(num, int) and 0 <= num and num <= 255:
            result = hex(num)[2:].upper()
            if len(result) == 1:
                result = '0' + result
            return result

    # intにもndarrayにもto_bytes()があるので、それにちなんで。1なのでlittleは意味はないが必須引数なので。(r g b alpha)ではなく(b g r alpha)であることに注意
    def to_bytes(self):
        return self.b.to_bytes(1, 'little') + self.g.to_bytes(1, 'little') + self.r.to_bytes(1, 'little') + self.alpha.to_bytes(1, 'little')





if __name__ == "__main__":
    a = bmp(1, 1, "000000")
    print(a)
    a.export("a.bmp")



    a = rgb("FF0F00")
    a.alpha = 0xA0
    print(a)



