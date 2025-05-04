
# .../my_lib_c/には、mymath.c, mymath.hなどが入っており、それらと一緒にコンパイルして、実行可能ファイルをBINDIRに出力するmake。デフォルトターゲットはカレントディレクトリ内すべての*.cをコンパイルする。もうひとつのktestのmakeとの違いはすべてのソースコードをコンパイルできる代わりに名前はそのままなので、テスト用ではなく、完成したcソースコードを一括でインストールする用。

CFILES := $(wildcard *.c)
TARGET := $(patsubst %.c,%,$(CFILES))

MAKEFILE_DIR := $(dir $(lastword $(MAKEFILE_LIST)))
PARENT_DIR := $(shell dirname ${MAKEFILE_DIR})
MYLIBDIR := $(PARENT_DIR)/language_c/library_c_handmade
MYLIBFILES := $(wildcard $(MYLIBDIR)/*.c)

# .hは.cと同じ場所。
INCDIR := $(MYLIBDIR)

BINDIR := ~/mybin/

CC = gcc
CFLAGS = -Wall -O3 -I $(INCDIR)

PROMPT_COMMAND = @printf "\e[0;38;2;255;165;0m$$\e[0;38;2;204;204;204m " 

.PHONY: all help
all: $(TARGET)

%: %.c $(MYLIBFILES)
	$(PROMPT_COMMAND)
	$(CC) $(CFLAGS) -o $@ $^
	$(PROMPT_COMMAND)
	mv $@ $(BINDIR)

help:
	$(PROMPT_COMMAND)
	cat $(firstword $(MAKEFILE_LIST))
