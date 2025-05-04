#!/usr/bin/env python3

'''
　バイナリ比較をfilecmpで行っているので、自作したい。ただpythonで自作すると遅いかも。filecmpはC言語apiつかってそう。
aiueo
aiueokaka
　でaとbというディレクトリにそれぞれtmp.txtといれてcmpしたところ、不一致とでた。
31 32 33
31 32 33 34
　となっていてもちゃんとバイナリ違うといってくれる。

　ほかにも、今は一致していればsame: 24と出るだけなのでいらないが、作りかけのsortのkeyの関数をつくりたい。abemaのllになりそう。あとll -Xの自作もできる。sizeを,区切りにしたい。
'''

import os
import sys
import filecmp

import progress

'''
　相対パス、あるいは絶対パスのディレクトリのstrを受け取って、再帰的に探索してフルパスのファイルのlistを返す関数。
　./を与えると、
./とa.txt
./とb.txt
./bとc.txt
　という感じなので、./を削って、
a.txt
b.txt
b/c.txt
　というようにする。./を削って、0文字でなければ/をつけて、あとは+する。フルパスの意味は、ファイルリストをディレクトリごとにつくるのではなく、パスを含めたファイルのリストをひとつつくるといういみ。
'''
def get_all_files(directory):
    full_path_file_lst = []
    for current_dir, sub_dir_lst, file_lst in os.walk(directory):
        dir_name = current_dir[len(directory):]
        if len(dir_name) != 0:
            dir_name += '/'
        for file_name in file_lst:
            full_path_file_lst.append(dir_name + file_name)
    return full_path_file_lst


# only_a, only_b, commonの3つのfull_path_file_nameのsort済みlistを返す関数。setにして処理してからlistにして返す。python.txtのset参照。sortはしていないので、呼び出し元で辞書順のソートなり、拡張子によるソートなりする。
def compare_directories(dir_a, dir_b):
    files_a = set(get_all_files(dir_a))
    files_b = set(get_all_files(dir_b))
    
    # Aにしかないファイル
    only_in_a = files_a - files_b 
    # Bにしかないファイル
    only_in_b = files_b - files_a
    # 両方に存在するファイルのバイナリ比較
    common_files = files_a & files_b

    return list(only_in_a), list(only_in_b), list(common_files)


# ls -Xのような拡張子によるsort。python.txtのlist.sort()参照。ここで、上に/で終わるもの、下にファイルというようにしたい。ls -Xのsortのkey関数自作。

# GPT参照。
'''
a.txt, b/p/q.txt, b/x.txt c.wav, d.txt
　とあったときに、最初にファイルを拡張子sortで表示して、次にディレクトリを深さ優先探索する順序でsortして表示したいです。つまり、これをソートすると
a.txt, d.txt, c.wav, b/x.txt, b/p/q.txt
　というふうになります。
(txt, a.txt)
(Z(/は最後), b, Z, p, txt, q.txt)
(Z, b, txt, x.txt)
(wav, c.wav)
(txt, d)
　かな。
'''


if __name__ == "__main__":

    if len(sys.argv) != 3:
        print("\
\n\
Usage: cmpdir.py ./handmade_mac/ ./handmade_uriel/\n\
this mean: cmpdir.py dir_a_name dir_b_name\n\
must be endwith '/'\n\
\n\
__main__: len(sys.argv) != 3"
        ) 
        exit()

    dir_a = sys.argv[1]
    dir_b = sys.argv[2]

    only_a, only_b, common = compare_directories(dir_a, dir_b)

    only_a.sort()
    only_b.sort()
    common.sort()


    print(f"────exist in only {dir_a}") 
    # 空の場合は改行なしで、たくさんあるときは改行してわかりやすくする。
    if len(only_a) != 0:
        print('\n'.join(only_a))
        print()

    print(f"────exist in only {dir_b}") 
    if len(only_b) != 0:
        print('\n'.join(only_b))
        print()

    print("────binary differ") 
    bin_same_lst = []
    bin_diff_lst = []
    bar = progress.progress(len(common), step=0)
    for i, file in enumerate(common):
        # dirをつけて相対パスにする。
        file_in_a = dir_a + file
        file_in_b = dir_b + file
        if filecmp.cmp(file_in_a, file_in_b, shallow=False):
            bin_same_lst.append(file)
        else:
            bin_diff_lst.append(file)
            print(file)
        bar.update(i)

    if len(bin_diff_lst) != 0: 
        print()

    print(f"binary same file: {len(bin_same_lst)}")


