
# .../my_lib_c/には、mymath.c, mymath.hなどが入っており、それらと一緒にコンパイルして、sample.cをコンパイルして、~/mybin/にktestという名前で実行可能ファイルを生成するmake。mybinはmybash.shでパスを通しているので、makeb sample && ktestでコンパイルと実行ができる。そのあとにsample2.cでmakeすればktestは上書きされる。あと、デフォルトターゲットはこの仕様上firstwordによってカレントディレクトリのうち先頭の*.cファイルにしている。カレントディレクトリにcがひとつだけならmakeb && ktestでcoderunnerみたいなことができる。

CFILE := $(firstword $(wildcard *.c))
TARGET := $(patsubst %.c,%,$(CFILE))

MAKEFILE_DIR := $(dir $(lastword $(MAKEFILE_LIST)))
PARENT_DIR := $(shell dirname ${MAKEFILE_DIR})
MYLIBDIR := $(PARENT_DIR)/c/library_c_handmade
MYLIBFILES := $(wildcard $(MYLIBDIR)/*.c)

# .hは.cと同じ場所。
INCDIR := $(MYLIBDIR)

BINFILE := ~/mybin/ktest

CC = gcc
CFLAGS = -Wall -O3 -I $(INCDIR)

PROMPT_COMMAND = @printf "\e[0;38;2;255;165;0m$$\e[0;38;2;204;204;204m " 

.PHONY: all help
all: $(TARGET)

%: %.c $(MYLIBFILES)
	$(PROMPT_COMMAND)
	$(CC) $(CFLAGS) -o $@ $^
	$(PROMPT_COMMAND)
	mv $@ $(BINFILE)

help:
	$(PROMPT_COMMAND)
	cat $(firstword $(MAKEFILE_LIST))
