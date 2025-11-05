#!/usr/bin/env python3

help = """
Usage: safety_remover.py a.txt b/c.txt ...
a.txt -> $SAFETY_REMOVER_DUMP_PATH/a.txt
b/c.txt -> $SAFETY_REMOVER_DUMP_PATH/c_20250922_132536.txt

__main__: len(sys.argv) < 2
"""[1:]



'''
### 概要
　rmの安全ver。$SAFETY_REMOVER_DUMP_PATHに引数で指定したファイルやディレクトリを移動するスクリプト。

　環境変数$SAFETY_REMOVER_DUMP_PATHを参照して、そこに削除したファイルを配置する。いわゆるゴミ箱である。
```shell
export SAFETY_REMOVER_DUMP_PATH="$HOME/dump"
```
を推奨する。


### build
```shell
rsync -avP uriel:j/software/handmade/python/safety_remover.py $BUILD_PATH/dist/handmade/bin/safety_remover.py
chmod 755 $BUILD_PATH/dist/handmade/bin/safety_remover.py
```
でインストールできる。

```
shutil.move(i, dump_dir)
mv a ~/dump/
```
とすると、タイムスタンプをつけられないので、
```
shutil.move(i, dist)
mv a ~/dump/a.txt
mv a ~/dump/a.txt_20241129_111421
```
とする必要がある。


　リムーブ機能を追加した。
```shell
$ /bin/rm ~/dump/*
```
を定期的にやるときに、すこし怖い。なので、
```shell
$ rr --clean
```
で$SAFETY_REMOVER_DUMP_PATH内をrmするようにした。とりあえずsubprocessで実装したが、できればshutilを使うのと、削除前にリストを取得して
```log
$ rr --clean
are you realy to remove this ?
/home/writer/dump/a.txt
/home/writer/dump/b/
/home/writer/dump/c.cpp
yes(Y) or no(N): Y
all removed.
```
というようにしたい。リストの要素に対して、ファイルならos.rm、サブディレクトリならshutil.rmtreeにする。python.mdのshutil参照。
'''

import shutil
import sys
import os
import time

if __name__ == '__main__':

    try:
        dump_dir = os.environ['SAFETY_REMOVER_DUMP_PATH'] 
    except KeyError:
        sys.stderr.write("Error: Environment variable SAFETY_REMOVER_DUMP_PATH is not set.\n")
        sys.exit(1)

    if len(sys.argv) < 2:
        print(help, end="") 
        exit(1)

    if len(sys.argv) == 2 and sys.argv[1] == "--clean":
        import subprocess
        subprocess.run(["/usr/bin/env rm -rfi $SAFETY_REMOVER_DUMP_PATH/*"], shell=True)
        exit(0)

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

