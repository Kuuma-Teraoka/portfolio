

### 概要
　コンパイルと実行まで1つのコマンドで行うためのスクリプト。cpp, java, typescript, latexに対応していて、拡張子で判別する。コンパイルが必要なプログラミング言語の学習に用いることを推奨している。

```shell
run test.cpp
```
で実行できる。この際、次のことが自動的に実行される。

1. ソースコードをコンパイルして、`runner_cpp_temp_binary`というファイル名で実行可能バイナリを生成
2. 実行可能バイナリを実行
3. `safety_remover.py`を用いて実行可能バイナリを削除


### install
```bash
rsync -avP uriel:j/software/handmade/bash/runner.sh $BUILD_PATH/dist/handmade/bin/run
chmod 755 $BUILD_PATH/dist/handmade/bin/run
```
でインストール。





### latex
```shell
$ lualatex -output-directory=$BUILD_PATH/build/handmade als.tex
```
でpdfが中間ファイルと一緒に`$BUILD_PATH/build/handmade`に生成されるので、pdfのみcpすればいい。[texlive.md]参照。

#### 必要な環境変数
`$RUNNER_BUILD_PATH` (`$BUILD_PATH/build/handmade`)





### typescript
　コマンドを見ればそこまで難しくない。

- $1: "~/inbox/hello.ts"
- ${1%.ts}: "~/inbox/hello"
- $(basename ${1%.ts}): "hello"

というように変換していることに注意。

### javascript
```shell
rsync -avP uriel:j/software/handmade/javascript/runner.js $BUILD_PATH/dist/handmade/bin/run
chmod 755 $BUILD_PATH/dist/handmade/bin/run
```
とするだけなので、runner.shで実装する必要ない。

### bash
```shell
rsync -avP uriel:j/software/handmade/bash/runner.sh $BUILD_PATH/dist/handmade/bin/run
chmod 755 $BUILD_PATH/dist/handmade/bin/run
```
とするだけなので、runner.shで実装する必要ない。

### python
```shell
rsync -avPu --delete uriel:j/software/handmade/python/lib_py_handmade/ $BUILD_PATH/dist/handmade/lib/python-handmade-packages/ -n
```
でライブラリ同期して、
```shell
rsync -avP uriel:j/software/handmade/python/ls.py $BUILD_PATH/dist/handmade/bin/ls
chmod 755 $BUILD_PATH/dist/handmade/bin/ls
```
とするだけなので、runner.shで実装する必要ない。







### java
　main, libraryという概念はないものの、自分をimportしている他のクラスが1つでもあるクラスは、すべてMainなしjarである`lib_java_handmade.jar`にまとめる。そしてこれを.aのように使って、cliで使いたいjavaのクラスをMainありjarとして、`lib_java_handmade.jar`のクラスも全て含めたjarにして、単独で使えるようにする。これはcppでの.aによってコンパイルした動的リンクのない、単独で動作する実行バイナリと同じである。

　ここで、できればエントリーポイントを実行したときに、必要となる最低限のclassのみjarにまとめたい。gradleなどにはこの機能はあるが、jarコマンドにはデフォルトでそのような機能はないらしい。しかたないので、`$CLASS_PATH`にあるclassをすべて`$RUNNER_BUILD_PATH/jar/`に展開して、エントリーポイントとなるrunの引数のclassも配置して、Manifestにエントリーポイントとしてrunの引数のclassを記述して、jarにまとめる。

　あとは、jarを
`$RUNNER_LIBEXEC_PATH` (`$BUILD_PATH/dist/handmade/libexec`)
に配置して、
```calender
#!/bin/bash
exec "${JAVA_HOME}/bin/java" -jar "${BUILD_PATH}/dist/handmade/libexec/calender.jar" "$@"
```
を`$RUNNER_BIN_PATH`に配置する。${}は必要ないが、一応つけた。

　とりあえず、単独で実行するバイナリとしてjarを生成するのは-mでの機能として、簡単な実行環境だけ作ろうと思う。
```shell
$ run $HOME/inbox/A.java
```
と実行すると、
```shell
$ javac -d $RUNNER_BUILD_PATH $HOME/inbox/A.java
$ java -cp $CLASSPATH:$RUNNER_BUILD_PATH A
```
を実行するようにする。$CLASSPATHには$BUILD_PATH/dist/handmade/lib/java/lib_java_handmade.jarも入っているので問題ない。

　-lで`$BUILD_PATH/dist/handmade/lib/java/lib_java_handmade.jar`を作成するようにする。

- `$RUNNER_JAVA_LIB_PATH ($DPATH/software/handmade/java/library_java_handmade)`
- `$RUNNER_LIB_JAR_PATH ($BUILD_PATH/dist/handmade/lib/java)`

に対して、
```shell
$ safety_remover.py $RUNNER_BUILD_PATH/jar
$ mkdir $RUNNER_BUILD_PATH/jar
$ javac -d $RUNNER_BUILD_PATH/jar $RUNNER_JAVA_LIB_PATH/*.java
```
というようにしてclassを配置して、
```shell
$ jar cf $RUNNER_LIB_JAR_PATH/lib_java_handmade.jar -C $RUNNER_BUILD_PATH/jar ./
```
で生成する。

- $1: "~/inbox/A.java"
- $(basename ${1%.java}): "A"

#### 必要な環境変数
- `$RUNNER_JAVA_LIB_PATH ($DPATH/software/handmade/java/library_java_handmade)`

- `$RUNNER_BUILD_PATH ($BUILD_PATH/build/handmade)`

- `$RUNNER_BIN_PATH ($BUILD_PATH/dist/handmade/bin)`
- `$RUNNER_LIBEXEC_PATH ($BUILD_PATH/dist/handmade/libexec)`
- `$RUNNER_LIB_JAR_PATH ($BUILD_PATH/dist/handmade/lib/java)`



### cpp
　動くようにした。hppのrsyncも上手くいく。--deleteは怖いのでつけていない。手動で削除してからrunすればすべてコピーするので、必要ないhppは消えるはず。

　日付確認して更新がなければスキップする機能をライブラリcppで実装したい。


- $1: "~/inbox/matpri.cpp"
- ${1%.cpp}: "~/inbox/matpri"
- $(basename ${1%.cpp}): "matpri"

というように変換していることに注意。"${!i}"についてはbash.mdの間接展開参照。

#### 必要な環境変数
- `$RUNNER_CPP_LIBHPP_PATH ($DPATH/software/handmade/cpp/library_cpp_handmade)`
- `$RUNNER_CPP_LIBCPP_PATH ($DPATH/software/handmade/cpp/library_cpp_handmade)`

- `$RUNNER_BUILD_PATH ($BUILD_PATH/build/handmade)`

- `$RUNNER_BIN_PATH ($BUILD_PATH/dist/handmade/bin)`
- `$RUNNER_INCLUDE_PATH ($BUILD_PATH/dist/handmade/include)`
- `$RUNNER_LIB_PATH ($BUILD_PATH/dist/handmade/lib)`


