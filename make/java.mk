# デフォルトターゲットはカレントディレクトリ内すべての*.javaをコンパイルする。

# javac Matrix.javaをすると、Matrix.class以外に、内部クラスのMatrix$MatrixIterator.classも生成される。
# なんかよくわからなくてshスクリプトでjmakeをつくった。そっちのほうが思い通りに動く。なんでわからなくなったかは、次のようだ。
# $ javac Matrix.java
# $ echo "Matrix$MatrixIterator.class a.class b.class"
# Matrix.class a.class b.class
# $ mv -v Matrix$MatrixIterator.class a.class b.class ~/mybin/java/
# copied 'Matrix.class' -> '/home/lucifer/mybin/java/Matrix.class'
# removed 'Matrix.class'
# copied 'a.class' -> '/home/lucifer/mybin/java/a.class'
# removed 'a.class'
# copied 'b.class' -> '/home/lucifer/mybin/java/b.class'
# removed 'b.class'
# $ echo "java -classpath ~/mybin/java/ Matrix" > ~/mybin/java/Matrix
# $ chmod 755 ~/mybin/java/Matrix

JFILES := $(wildcard *.java)
TARGET := $(patsubst %.java,%.class,$(JFILES))

BINDIR := ~/mybin/java

PROMPT_COMMAND = @printf "\e[0;38;2;255;165;0m$$\e[0;38;2;204;204;204m " 

.PHONY: all help
all: $(TARGET)

%.class: %.java
	$(PROMPT_COMMAND)
	javac $^
	CLASSFILES=$(shell find . -name '*.class')
	$(PROMPT_COMMAND)
	mv -v $(CLASSFILES) $(BINDIR)/
	$(PROMPT_COMMAND)
	echo "java -classpath $(BINDIR)/ $(patsubst %.class,%,$@)" > $(BINDIR)/$(patsubst %.class,%,$@)
	$(PROMPT_COMMAND)
	chmod 755 $(BINDIR)/$(patsubst %.class,%,$@)


help:
	$(PROMPT_COMMAND)
	cat $(firstword $(MAKEFILE_LIST))
