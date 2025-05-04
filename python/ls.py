#!/usr/bin/env python3

import os
import stat
import pwd
import grp
import time

import sys

import strings

'''
　a.txtを、('txt', 'a.txt')というtupleに変換する関数。list.sort()のkey引数に渡すためのmethod。

　拡張子のないディレクトリや実行可能ファイルについては、ディレクトリは上、実行可能ファイルは下にあるファイルたちのうち、上に来るようにする。なので、ディレクトリなら、('0', 'creativity')というようにして、実行可能ファイル、つまり.のないファイルなら('1', 'print_bin')というようにする。txtなどの拡張子で、'0'(30)や'1'(31)より早い文字が来ることはないと思うので。

　abemaのためのソートも導入。
第51話 劇場型悪意 [25-139_s3_p51].mp4
　という名前で、('25-139', 3, 51)というtupleを渡せばseries、season、episodeの順にsortできる。

　ここで、[, _, _, ]が含まれるmp4ファイルの場合に、('mp4', '25-139', 3, 51)を返すようにする。sortでは、
(3, '4')
(2, 3.5)
　のような場合に3と2は比較できるが、'4'と3.5では、'4'.__gt__(3.5)がTypeerrorをRaiseしてしまう。なので、ほかの(str, str)とは違う、(str, str, int, int)というtupleを返すのは怖いのだが、'25-139'が一致してはじめて次の3を比較するので、
第51話 劇場型悪意 [25-139_s3_p51].mp4
25-139.mp4
　というようなファイルが存在しない限り大丈夫。
'''
def method_for_sort(name):
    if os.path.isdir(name):
        return ('0', name)
    dot_index = name.rfind('.') 
    if dot_index != -1:
        suffix = name[dot_index + 1 : ]
        if suffix == 'mp4':
            index = [name.rfind(']')]
            index.append(name[:index[0]].rfind('_'))
            index.append(name[:index[1]].rfind('_'))
            index.append(name[:index[2]].rfind('['))
            # mp4で[25-139_s3_p51]を含むとき。また、[ynn___2002]などある場合にint('')でエラーになるので[x_x_x]でxが1文字以上の場合。
            if index.count(-1) == 0 and index[0] - index[1] != 1 and index[1] - index[2] != 1 and index[2] - index[3] != 1:
                series = name[index[3] + 1 : index[2]]
                # _s3_のsや_p51_のpを抜くための+1
                season = name[index[2] + 1 + 1 : index[1]]
                episode = name[index[1] + 1 + 1 : index[0]] 
                return (suffix, series, int(season), int(episode))
            # mp4で[25-139_s3_p51]が含まれないとき
            else:
                return (suffix, name)
        # mp4以外の.ありファイル
        else:
            return (suffix, name)
    # .なしファイル。つまり実行可能ファイル
    else:
        return ('1', name)

# lll用。つまりsuffixではなく名前のみでのsort。ディレクトリは上に表示する。
def method_2(name):
    if os.path.isdir(name):
        return ('0', name)
    else:
        return ('1', name)



if __name__ == '__main__':
 
    x_flag = False

    i = 1
    while(i < len(sys.argv)):
        if sys.argv[i] == "-X":
            x_flag = True
        else:
            break
        i += 1

    if len(sys.argv) - i == 0:
        dir_name = './'
    elif len(sys.argv) - i == 1:
        dir_name = sys.argv[i] if sys.argv[i].endswith("/") else sys.argv[i] + "/"
    else:
        print('Usage:')
        exit()


    file_dir_lst = os.listdir(dir_name)
    if len(file_dir_lst) == 0:
        print('this dir is empty.')
        exit()


    if x_flag:
        file_dir_lst.sort(key=method_for_sort)
    else:
        file_dir_lst.sort(key=lambda name: ('0' if os.path.isdir(name) else '1', name))


    max_size_len = max(len(strings.comma(os.stat(dir_name + i).st_size)) for i in file_dir_lst)

    for index, i in enumerate(file_dir_lst):
        i_status = os.stat(dir_name + i)
        print(
            f'{index + 1 :>{len(str(len(file_dir_lst)))}}',
            stat.filemode(i_status.st_mode)[1:10],
            pwd.getpwuid(i_status.st_uid).pw_name,
            grp.getgrgid(i_status.st_gid).gr_name,
            time.strftime('%Y%m%d_%H%M%S', time.localtime(i_status.st_mtime)),
            f"{strings.comma(i_status.st_size):>{max_size_len}}",
            f'\033[38;5;117m{i}\033[0m' if os.path.isdir(dir_name + i) else i
        )




