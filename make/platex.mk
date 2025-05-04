
# 　open sourceのビルドのように、ソースコードディレクトリ、ビルドディレクトリ、インストールディレクトリに分けて考えたい。ようは、
# $ tmake ~/j/academic/.../article/als.tex
# 　とすると、aliasで、
# $ make -f $HANDMADE_PATH/make/platex.mk -C $BUILD_PATH/build/handmade ~/j/academic/.../article/als.tex
# にかわって、ソースコードディレクトリを~/j/academic/.../articleとして、~/j/academic/.../article/img.pngやらを、いや、texでimg.pngとかいているので、カレントディレクトリはソースディレクトリである必要がある。

# ここで、
# $ platex -output-directory=$BUILD_PATH/build/handmade als.tex
# 　とすると、dviなどのファイルがすべて$BUILD_PATH/build/handmadeに生成されるようになる。ただ、dvipdfmxにはこのようなオプションはなく、カレントディレクトリにpdfを生成する。ここで実験したところ、
# $ dvipdfmx $BUILD_PATH/build/handmade/sample.dvi
# 　でpdfはカレントディレクトリに生成された。つまり、この二つのコマンドで目的の挙動になっている。



ifeq ($(BUILD_PATH),)
$(error BUILD_PATH is not set. Please set it before running make.)
endif

TEXFILES := $(wildcard *.tex)
TARGET := $(patsubst %.tex,%.pdf,$(TEXFILES))
PROMPT_COMMAND = @printf "\e[0;38;2;255;165;0m$$\e[0;38;2;204;204;204m " 
PLATEX = platex -output-directory=$(BUILD_PATH)/build/handmade

.PHONY: all clean help

all: $(TARGET)
%.pdf: %.tex
	$(PROMPT_COMMAND)
	$(PLATEX) $< && $(PLATEX) $<
	$(PROMPT_COMMAND)
	dvipdfmx $(BUILD_PATH)/build/handmade/$(patsubst %.tex,%.dvi,$<)
clean:
	$(PROMPT_COMMAND)
	rm.py $(BUILD_PATH)/build/handmade/*
help:
	$(PROMPT_COMMAND) 
	cat $(firstword $(MAKEFILE_LIST))

