
ifeq ($(BUILD_PATH),)
$(error BUILD_PATH is not set. Please set it before running make.)
endif

TEXFILES := $(wildcard *.tex)
TARGET := $(patsubst %.tex,%.pdf,$(TEXFILES))
PROMPT_COMMAND = @printf "\e[0;38;2;255;165;0m$$\e[0;38;2;204;204;204m "
LATEX = lualatex -output-directory=$(BUILD_PATH)/build/handmade

.PHONY: all help clean

all: $(TARGET)
%.pdf: %.tex
	#$(PROMPT_COMMAND)
	#comma_period_replacer.sh $<
	$(PROMPT_COMMAND)
	$(LATEX) $< #&& $(LATEX) $<
	$(PROMPT_COMMAND)
	cp -fv $(BUILD_PATH)/build/handmade/$(patsubst %.tex,%.pdf,$<) ./
clean:
	$(PROMPT_COMMAND)
	rm.py $(BUILD_PATH)/build/handmade/*
help:
	$(PROMPT_COMMAND) 
	cat $(firstword $(MAKEFILE_LIST))

