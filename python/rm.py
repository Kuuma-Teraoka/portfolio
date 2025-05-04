#!/usr/bin/env python3

import shutil
import sys
import os
import time

'''
　DUMP_PATHに引数で指定したファイルやディレクトリを移動するスクリプト。
shutil.move(i, dump_dir)
mv a ~/dump/
　とすると、タイムスタンプをつけられないので、
shutil.move(i, dist)
mv a ~/dump/a.txt
mv a ~/dump/a.txt_20241129_111421
　とする必要がある。
'''

if __name__ == '__main__':

    dump_dir = os.environ['DUMP_PATH'] 

    if len(sys.argv) < 2:
        print(
            "\n"
            "Usage: rm.py a.txt b/c.txt\n"
            "\n"
            "__main__: len(sys.argv) < 2"
        )
        exit()

    for i in sys.argv[1:]:
        if not os.path.exists(i):
            print(f"'{i}': No such file or directory")

        else:
            #print(f"'{i}'", end='')

            # a/b.txt c/ -> a/b.txt c
            if len(i) > 1 and i[-1] == '/':
                i = i[:-1]
                #print(f" -> '{i}'", end='')

            # a/b.txt c -> b.txt c
            dist = i.split('/')[-1]
            #print(f" -> '{dist}'", end='') 

            # b.txt c -> ('b','.txt') ('c','') -> ('b_2024...931','.txt') ('c','') -> b_2024...931.txt, c
            if os.path.exists(dump_dir + '/' + dist):
                extensions = ""
                if (priod := dist.rfind('.')) != -1:
                    extensions = dist[priod:]
                    dist = dist[:priod] 
                #print(f" -> ('{dist}', '{extensions}')", end='')
                dist = dist + '_' + time.strftime('%Y%m%d_%H%M%S', time.localtime())
                #print(f" -> ('{dist}', '{extensions}')", end='') 
                dist = dist + extensions
                #print(f" -> '{dist}'", end='')

            # b.txt c -> dump/b.txt dump/c
            dist = dump_dir + '/' + dist
            #print(f" -> '{dist}'")
 
            # 先にshutil.moveするとisdirがFalseになってしまう。
            if os.path.isdir(i):
                print(f"'{i}/' -> '{dist}/'")
            else:
                print(f"'{i}' -> '{dist}'")
            shutil.move(i, dist)

