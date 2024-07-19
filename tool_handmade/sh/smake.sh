#!/bin/bash

if [ "$#" -ne 1 ]; then
    echo "Usage: $0 "
    exit 1
fi

# ~/d/sh/a.shなら、~/d/sh/を消して、.shを消して、aを抜き出す。
TARGET=$(echo $1 | sed 's#.*/##; s/\.[^.]*$//')

echo -en "\e[0;38;5;214m$\e[0;37m "
echo "cp -fv \"$1\" ~/mybin/\"$TARGET\""
cp -fv "$1" ~/mybin/"$TARGET"