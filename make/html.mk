
# aliasでいいかも。ただ、makeだとコマンドが表示されるのと、htmlのコンパイルという形式的な意味も持たせたい。詳しくはdata.txtの各ファイルの扱い方を参照。
# index.htmlが共通の名前なので上書きされない場合がある。同期というより、すべて削除してからすべて送るがいいかも。rsyncより、rm *とcp *。

PROMPT_COMMAND = @printf "\e[0;38;2;255;165;0m$$\e[0;38;2;204;204;204m " 

.PHONY: all help

all:
	$(PROMPT_COMMAND)
	rm -rv /var/www/html/* && cp -rv ./* /var/www/html/

vps:
	$(PROMPT_COMMAND)
	rsync -avu -e "ssh -p 2222" ./ lucifer@162.43.27.214:/var/www/html/ --delete

help:
	$(PROMPT_COMMAND)
	cat $(firstword $(MAKEFILE_LIST))