#!/bin/bash

# mv *.JPG *.jpg
# というようなときにつかう。もちろんこのコマンドは意味をもたない。そこで、sedの置換のような形式で使おうと思う。

# Check if the correct number of arguments is provided
if [ "$#" -ne 2 ]; then
    echo "Usage: $0 21TI014_BasDatSci_(.*)\.pdf report_\1.pdf"
    exit 1
fi

for file in $(ls); do
  new_name=$(echo "$file" | sed -E "s/$1/$2/")
  echo -en "\e[0;38;5;214m$\e[0;37m "
  echo "mv -iv \"$file\" \"$new_name\""
  mv -iv "$file" "$new_name"
done
