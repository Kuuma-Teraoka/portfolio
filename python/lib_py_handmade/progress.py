#!/usr/bin/env python3


'''
import progress
bar = progress.progress(100)
for i in range(100)
    bar.update(i)
　というように使う。





　前のprogressでは-を表示していくので少し複雑だったが、\rをつかうことで毎回表示をリセットするので簡単になった。
　なぜそれを使わなかったかや\rのテキストエディタやターミナルでの振る舞いについてはfile_format/log.txtを参照。


$ pip install numpy
Collecting numpy
  Downloading numpy-2.1.3-cp310-cp310-manylinux_2_17_x86_64.manylinux2014_x86_64.whl (16.3 MB)
     ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━ 16.3/16.3 MB 1.2 MB/s eta 0:00:00
Installing collected packages: numpy
Successfully installed numpy-2.1.3
　を参考にしたい。

'━': E2 94 81
'─': E2 94 80
　で、E2 94 80はいつもつかっているけいせんである。E2 94 81はそれの太いバージョンなので、それにより進捗を表示している。色をつける場合でもこれを使ったほうがきれいだと思う。

終了後の線の色: 4E AD 2A -> 95 AF 95 = 71 
太線の色: FF 00 84 -> FF 00 87 = 198
細線の色: 3A 3A 3A = (58,58,58) = 237
16.3/16.3 MB: 00 81 00 -> 00 87 00 = 28
1.2 MB/s: CC 00 00 -> D7 00 00 = 160 
0:00:00: 00 9D 9B -> 00 AF AF = 37  


$ pv -btrpe
250MiB(b) 0;00;15(t) [24.5MiB/s](r) [======>       ] 40%(p) ETA 0:00:50(e)
　も参考になるかと思ったがpip installのほうがきれいなのでこっちはいい。あとgradrewもデザインよかった気がする。いやあれはたしか----10%スタイルだったのでそうでもないかも。

$ curl -O https://ftp.gnu.org/gnu/gcc/gcc-13.2.0/gcc-13.2.0.tar.gz
  % Total    % Received % Xferd  Average Speed   Time    Time     Time  Current
                                 Dload  Upload   Total   Spent    Left  Speed
 20  146M   20 30.1M    0     0   661k      0  0:03:46  0:00:46  0:03:00  711k
　これもわかりやすいかも。Average DloadとCurrent Speedを表記するのはいいと思う。ただ個人的にはバーが欲しい。




　stepは動作を軽くするためのもの。0.1%ずつ表示を返れば十分きれいなので、それ以外のときはupdateの先頭でreturnして軽くする。



　cppと同様に、インスタンスにした。20241217現在cppとコードは完全に一致している。

　クラスにしてインスタンスを扱うことにより、main()でprogress timeとしておけばプログラム全体の時間を測ることができるうえ、ちゃんと関数内でprogress timeとしても別のインスタンスが作られるだけなので、グローバル変数のようにstart_timeが上書きされる心配もない
　updateではmax360であれば0,1,...,359という入力を、1,2,...,360に変換している。なので、引数のカウントは0,1,...,359が前提になっている。もし、1,2,..,360を入れたいのであればupdate(i-1)にして使う。
　ytdlのようにバイト数を扱うときに、0,1,...,360を入れたいのであればmaxを+1にしてもいいが合計バイト数が変わるのも気になるので、update(byte-1)をいれて、byteが0だと-1になってしまうが、0では呼び出さないようにする。

'''

import time
import tracemalloc
import sys

import strings


class progress:
    def __init__(self, max:int=100, label:str="", step:int=1000, bar_length:int=40):
        self.max = max
        self.step = step if step < self.max else 0
        self.label = label
        self.bar_length = bar_length
        self.start_time = time.perf_counter()
        sys.stdout.write("\x1b[?25l")
        sys.stdout.flush()

    def __del__(self):
        sys.stdout.write("\x1b[0m\x1b[?25h")

    def elapsed(self):
        self.current_time = time.perf_counter()
        return self.current_time - self.start_time

    def display(input):
        min = int(input / 60)
        sec = int(input % 60)
        return f"{min//10}{min%10}:{sec//10}{sec%10}"

    def update(self, current):
        current += 1
        if self.step != 0 and current != 1 and current != self.max and current % (self.max // self.step) != 0:
            return
        elap = self.elapsed()
        if 0 < current and current < self.max:
            bar_num = int(current / self.max * self.bar_length)
            percentage = current / self.max * 100
            eta = elap / current * self.max 
            sys.stdout.write(
                f"\r\x1b[2K"
                f"{self.label}"
                f"     \x1b[38;5;198m{'━' * bar_num}"
                f"\x1b[38;5;237m{'─' * (self.bar_length - bar_num)}"
                f" \x1b[38;5;37m{percentage:.1f}%"
                f" \x1b[38;5;28m{strings.comma(current)}/{strings.comma(self.max)}"
                f" \x1b[38;5;28m{strings.comma(int(current/elap))}/s"
                f" \x1b[0meta \x1b[38;5;37m{type(self).display(elap)}/{type(self).display(eta)}"
                f"\x1b[0m"
            )
            sys.stdout.flush() 
        elif current == self.max:
            sys.stdout.write(
                f"\r\x1b[2K"
                f"{self.label}"
                f"     \033[38;5;71m{'━' * self.bar_length}"
                f" \x1b[38;5;37m100%"
                f" \x1b[38;5;28m{strings.comma(current)}/{strings.comma(self.max)}"
                f" \x1b[38;5;28m{strings.comma(int(current/elap))}/s"
                f" \x1b[0melapsed: \x1b[38;5;37m{progress.display(elap)}"
                f"\x1b[0m\n\x1b[?25h"
            )
        else:
            raise ValueError("progress.update(): current <=0 or max < current")



if __name__ == "__main__":

    max = 600000
    interbal = 5 / max
    bar = progress(max, "main():")
    for i in range(max):
        if i % 100 == 0:
            time.sleep(interbal * 100)
        bar.update(i)






















'''
　以下、過去の。

　max=385なら、current=0で時間計測開始でcurrent=384で100%と合計時間表示して改行。なのでこのふたつは必ず踏む必要あり。
　stepは0,1,...,40032, 40033が与えられたときに、step=1000とすると、40033//1000=40の倍数でないときはskipするというもの。0と1とmax-1はskipしないようにしてる。1は進捗表示の最初なので。
　displayは秒数のfloat64をstrに、elapseはstart_time_updateというグローバル変数を参照して経過時間をfloat64で返す関数だが、クラスのインスタンスにしてもいいかもしれない。そこらへんやればもっとわかりやすくなると思う。




start_time_update = None
start_time_total = time.perf_counter()
tracemalloc.start()

# start_time_updateを今の時間にする関数
def start():
    global start_time_update
    start_time_update = time.perf_counter()

# start_time_updateからの経過時間をsec単位のfloat64で返す関数
def elapsed():
    return time.perf_counter() - start_time_update
# start_time_totalからの経過時間をsec単位のfloat64で返す関数
def total_time():
    return time.perf_counter() - start_time_total

# sec単位のfloat64を受け取って、38:00というmin:secのstrに変換して返す関数
def display(input_time):
    #if input_time < 60:
    #    return f'{input_time:.2f} [sec]'
    #else:
        minite = int(input_time // 60)
        second = int(input_time % 60)
        return f'{minite // 10}{minite % 10}:{second // 10}{second % 10}'

# 上記参照。
def update(current, max, bar_length=40, step=None, label=''):
    # max360のときの0,1,...,359を、1,2,...,360にする。
    current += 1

    if step != None and current != 1 and current != 2 and current != max and current % (max // step) != 0:
        return

    if 1 < current and current < max:
        progress = int(current / max * bar_length) 
        bar = '\033[38;5;198m' + '━' * progress + '\033[38;5;237m' + '─' * (bar_length - progress) + '\033[0m'
        percentage = current / max * 100
        eta = elapsed() / (current-1) * (max - current) # 1が終わってからの時間なので
        sys.stdout.write(
            f"\r\033[2K{label}"
            f"     {bar}"
            f" \033[38;5;28m{percentage:.1f}%\033[0m"
            f" \033[38;5;28m{strings.comma(current)}/{strings.comma(max)}\033[0m"
            f" eta \033[38;5;37m{display(eta)}\033[0m"
        )
        sys.stdout.flush()
    elif current == 1:
        start()
        sys.stdout.write('\033[?25l')
    elif current == max:
        sys.stdout.write(
            f"\r\033[2K{label}"
            f"     \033[38;5;71m{'━' * bar_length}\033[0m"
            f" \033[38;5;28m100%\033[0m"
            f" \033[38;5;28m{strings.comma(current)}/{strings.comma(max)}\033[0m"
            f" elapsed: \033[38;5;37m{display(elapsed())}\033[0m\n"
            f"\033[?25h"
        )
    else:
        raise ValueError("progress.update(): else")

# import時に初期化されるstart_time_totalからの経過時間を標準出力する関数。
def total():
    print(f'total calculation time: {display(total_time())}')


# peak時、現在のメモリ使用量に関する関数。python.txt参照。
def peak_memory():
    return tracemalloc.get_traced_memory()[1]
def current():
    print(f"current memory usage: {strings.comma(tracemalloc.get_traced_memory()[0])} [byte]")
def peak():
    print(f"peak memory usage: {strings.comma(peak_memory())} [byte]")



'''








# 過去のprogress.update()
'''
　max = 24で、p(i) = i / max * 100、q(i) = (i + 1) / max * 100とすると、
i: 0, 1, 2, ..., 23
p(i): 0, 4.16, 8.13, ..., 95.83
int(p(i)): 0, 4, 8, .., 95
q(i): 4.16, 8.13, 12.5, .., 100
int(q(i)): 4, 8, 12, ..., 100
　なので、[int(p(i)), int(q(i)))の整数を数えると、
i=0: 0,1,2,3
i=1: 4,5,6,7
i+2: 8,9,10,11
...
i=24: 95,96,97,98,99
　となる。for j in range(int(p(i)), int(q(i))):にして、j+1は1~100まで移動するので、(j + 1) % 10 == 0なら%を出力すればいい。
'''

'''
def update(i, max):
    if isinstance(i, int) and isinstance(max, int):
        if i == 0:
            start()
            print("<", end="", flush=True)
        for j in range(int(i / max * 100), int((i + 1) / max * 100)):
            if (j + 1) % 10 == 0:
                print(f'{j + 1}%', end="", flush=True)
            else:
                print("─", end="", flush=True)
        if i == max - 1:
            print(f'> complete in {end()}')
    else:
        raise TypeError("else")
'''



