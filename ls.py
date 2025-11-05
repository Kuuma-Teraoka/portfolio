#!/usr/bin/env python3

import os
import stat
import pwd
import grp
import time

import subprocess

import sys

import strings


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
      index.append(name.rfind('['))
      # mp4で[a_s1_p1]を含むとき
      if index.count(-1) == 0 and index[0] - index[1] > 1 and index[1] - index[2] > 1 and index[2] - index[3] > 1:
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
  # 可変長コマンドライン引数におけるオプション。else breakでオプション以外の文字列がきた時にループを抜けて、それ以降を可変長コマンドライン引数として処理する。ls.pyにおいては、0個であれば"./"、1個であればその文字列をディレクトリのパスとして処理する。
  x_flag = False
  d_flag = False

  i = 1
  while(i < len(sys.argv)):
    if sys.argv[i] == "-X":
      x_flag = True
    elif sys.argv[i] == "-d":
      d_flag = True
    else:
      break
    i += 1

  if len(sys.argv) - i == 0:
    dir_name = './'
  elif len(sys.argv) - i == 1:
    dir_name = sys.argv[i] if sys.argv[i].endswith("/") else sys.argv[i] + "/"
  else:
    print('Usage:')
    exit(1)


  file_dir_lst = os.listdir(dir_name)
  if len(file_dir_lst) == 0:
    print('this dir is empty.')
    exit(1)


  if x_flag:
    file_dir_lst.sort(key=method_for_sort)
  else:
    file_dir_lst.sort(key=lambda name: ('0' if os.path.isdir(name) else '1', name))

  size_lst = []
  for name in file_dir_lst:
    if os.path.islink(dir_name + name):
      size_lst += [0]
    elif d_flag and os.path.isdir(dir_name + name):
      size_lst += [int(subprocess.run(["du", "-sb", dir_name + name], capture_output=True, text=True).stdout.split("\t")[0])]
    else:
      size_lst += [os.stat(dir_name + name).st_size]

  max_size_len = max(len(strings.comma(i)) for i in size_lst)

  for index, name in enumerate(file_dir_lst): 
    if os.path.islink(dir_name + name):
      status = os.lstat(dir_name + name)
    else:
      status = os.stat(dir_name + name)

    link = ''
    if not os.path.isdir(dir_name + name) and status.st_nlink >= 2:
      link = f'\033[38;5;201m<- {status.st_nlink}\033[0m'
    if os.path.islink(dir_name + name):
      try:
        link = f'\033[38;5;201m-> {os.readlink(dir_name + name)}\033[0m' 
      except OSError as e:
        link = f'\033[38;5;201m-> ??? ({e})\033[0m'

    print(
      f'{index + 1 :>{len(str(len(file_dir_lst)))}}',
      stat.filemode(status.st_mode)[1:10],
      pwd.getpwuid(status.st_uid).pw_name +
      ":" +
      grp.getgrgid(status.st_gid).gr_name,
      time.strftime('%Y%m%d_%H%M%S', time.localtime(status.st_mtime)),
      f"{strings.comma(size_lst[index]):>{max_size_len}}",
      f'\033[38;5;117m{name}/\033[0m' if os.path.isdir(dir_name + name) else name,
      link
    )

