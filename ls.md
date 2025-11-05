

### 概要
　基本的には`ls -la`と同様にカレントディレクトリの中身を表示するプログラムである。`ls -la`の表示を自分好みにカスタマイズしようとしたところ、限界があったので、pythonのos.statライブラリを使って1から作成した。

　コンセプトとしては、
```log
$ ll academic/other/
 1 rwxr-xr-x uriel:uriel 20250315_175443  4,096 animal_savedata_3ds_20250315_174014/
 2 rwxrwxr-x uriel:uriel 20250309_231423  4,096 download/
 3 rw-rw-r-- uriel:uriel 20241026_172724  6,788 angel.txt <- 2
 4 rw-rw-r-- uriel:uriel 20241026_172726  2,044 braille.txt <- 2
...
10 rw-rw-r-- uriel:uriel 20250524_123452 97,101 a.txt -> /home/uriel/j/things_to_do.txt
```
というように、ディレクトリは末尾に/あり、青色、最初に表示。ファイルは白色で表示する。また、symlinkはls -laと同様に->で宛先を示し、色はピンクに。hardlinkはsymlinkとは逆向きに、`<- link数`と表示するようにした。矢印の向きはただsymlinkと対応づけたかっただけで特に意味はない。link数が1でない、つまりhardlinkのときにlink数を表示したいだけ。


### `ls -la`とのちがい

- 数字があるのでファイル数がすぐにカウントできる
- rwxrwxrwxの先頭にlとかdとかを加えるのではなく、ファイル、ディレクトリ名に/や->を加えることで判別できるようにした
- link数は1でないものだけ<- link数で表示する
- ファイルサイズをbyte単位でcomma区切りにしている
- 色や順番を自分好みに


### to do
　lsで表示される要素をclassで定義して、その配列を扱うようにしたほうがいいと思う。あとでやる。


### install
```shell
rsync -avP uriel:j/software/handmade/python/ls.py $BUILD_PATH/dist/handmade/bin/ls.py
chmod 755 $BUILD_PATH/dist/handmade/bin/ls.py
```
でインストールできる。

### link
　symlink, hardlinkの扱いが難しい。os.path.islink()はsymlinkのみtrueになるので、symlinkはこれで区別できる。hardlinkはstatus.st_nlink >= 2で判別できるが、これはdirも対象になってしまうので、
```py
if not os.path.isdir(dir_name + name) and status.st_nlink >= 2:
```
というようにして判別している。ちなみにsymlinkのdirもあるので、symlinkではdirにも対応させている。

　また、os.statにsymlinkのpathを渡すと宛先の情報を取得する。宛先が存在しない場合はエラーになる。なので、symlinkだけos.lstat()でsymlink自体の情報取得に変更した。



- ["academic/", "a.txt", "b.txt", ...]
- [24, 34523, 45523]

というように、file, directoryのnameのstringのlistと、sizeのintのlistを順序で対応させて、最初に取得しておき、最大サイズに合わせて空白を調整する。




### `method_for_sort(name)`
　a.txtを、('txt', 'a.txt')というtupleに変換する関数。list.sort()のkey引数に渡すためのmethodである。

1. ディレクトリ
2. 拡張子のない実行可能ファイル
3. 拡張子のあるファイル

という順で表示するようにtupleを構成する。なので、ディレクトリなら、('0', 'creativity')というようにして、拡張子のない実行可能ファイルなら('1', 'print_bin')というようにする。txtなどの拡張子で、'0'(30)や'1'(31)より早い文字が来ることはないと思うので。
　
`第1話 タイトル [111-21_s2_p1].mp4`
というように拡張子の前に[a_b_c]というタグがついたファイル名をsortできるようにしている。('111-21', 2, 1)というtupleを渡せばseries、season、episodeの順にsortできる。

　ここで、[, _, _, ]が含まれるmp4ファイルの場合に、('mp4', '111-21', 2, 1)を返すようにする。sortでは、
(3, '4')
(2, 3.5)
　のような場合に3と2は比較できるが、'4'と3.5では、'4'.__gt__(3.5)がTypeerrorをRaiseしてしまう。なので、ほかの(str, str)とは違う、(str, str, int, int)というtupleを返すのは怖いのだが、'25-139'が一致してはじめて次の3を比較するので、

- 第1話 タイトル [111-21_s2_p1].mp4
- 111-21.mp4

というようなファイルが存在しない限り大丈夫。また、[a___200]などある場合にint('')でエラーになるので[x_x_x]でxが1文字以上の場合にしている。














