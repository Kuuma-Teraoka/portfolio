#!/usr/bin/env python3

import struct

import strings

used_rand_file_size = 0

def used():
    print(f'random file usage: {strings.comma(used_rand_file_size)} [byte]')

# 乱数ファイルの読み込みはすべてluを元に行われるので、この関数でモジュール変数をインクリメントすれば乱数ファイル使用量を計算できる。
def lu(rand_file):
    global used_rand_file_size
    used_rand_file_size += struct.calcsize("L")
    ran_byte = rand_file.read(struct.calcsize("L"))
    ran_int = struct.unpack("L", ran_byte)[0]
    return ran_int

def f01(rand_file):
    max = 2 ** (8 * struct.calcsize("L"))
    return lu(rand_file) / max

def fcustom(rand_file, a, b):
    if isinstance(a, float) or isinstance(a, int) and isinstance(b, float) or isinstance(b, int):
        return (f01(rand_file) * (b - a) + a)
    else:
        raise TypeError("else")

if __name__ == "__main__":

    with open("/home/lucifer/mybin/randfile_s2_1g.bin", "rb") as rand_file:
        a = 0
        for i in range(10000):
                if f01(rand_file) > 0.9:
                    a += 1
    
    print(a)

    with open("/home/lucifer/mybin/randfile_s2_1g.bin", "rb") as rand_file:
        a = 0
        for i in range(10000):
                if f01(rand_file) < 0.2:
                        a += 1
    print(a)
    