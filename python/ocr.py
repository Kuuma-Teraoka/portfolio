"""
$ . ~/env3.10/bin/activate
$ python3 ./ocr.py picture.jpg
　でpicture.jpgを読み込んで、認識した文字列を標準出力する。

software/open_source/capture_editing_playback/ocr.txt
　参照。
"""

import easyocr
import sys
reader = easyocr.Reader(['ja', 'en'])
text = reader.readtext(sys.argv[1], detail=0)
print('\n'.join(text))

