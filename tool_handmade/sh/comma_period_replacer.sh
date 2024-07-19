#!/bin/bash
#--------------------------------------------------
# Script Name: punctuation_converter.sh
# Description: このスクリプトは、日本語のテキストファイル内の句読点を変換します。
#              具体的には、「、」を「，」に、「。」を「．」に変換します。
# Author: 寺岡空馬
# Date: 2024年7月5日
# Usage: ./punctuation_converter.sh input_file output_file
#--------------------------------------------------

# chatGPTに聞いたらいいのを送ってくれた。意味はbash/grammer.txtやcommand.txtを見れば完全理解できるはず。

# Check if the correct number of arguments is provided
if [ "$#" -ne 1 ]; then
    echo "Usage: $0 input_file"
    exit 1
fi

input_file="$1"

# Create a temporary file
temp_file=`mktemp`

# Convert punctuation and save to temporary file
sed 's/、/，/g; s/。/．/g' "$input_file" > "$temp_file"

# Move temporary file to the output file
mv "$temp_file" "$input_file"

echo "Conversion complete."
