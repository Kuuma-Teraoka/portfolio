

# a.tex, b.tex, c.texが入ったフォルダでmakeすると、a.tex, a.pdf, b.tex, b.pdf, c.tex, c.pdfにするmake。

TEXFILES := $(wildcard *.tex)
TARGET := $(patsubst %.tex,%.pdf,$(TEXFILES))

LATEX = platex
DVIPDFMX = dvipdfmx

PROMPT_COMMAND = @printf "\e[0;38;2;255;165;0m$$\e[0;38;2;204;204;204m " 

.PHONY: all help
all: $(TARGET)

# $<=$(firstword $^)はいいとして、1wordなので$^でも$<でも変わらない。
%.pdf: %.dvi
	$(PROMPT_COMMAND) 
	$(DVIPDFMX) $<
	$(PROMPT_COMMAND) 
	rm $<

%.dvi: %.tex
	$(PROMPT_COMMAND) 
	~/mybin/comma_period_replacer $<
	$(PROMPT_COMMAND) 
	$(LATEX) $<
	$(PROMPT_COMMAND) 
	rm $(patsubst %.tex,%.aux,$<) $(patsubst %.tex,%.log,$<)

help:
	$(PROMPT_COMMAND) 
	cat $(firstword $(MAKEFILE_LIST))
