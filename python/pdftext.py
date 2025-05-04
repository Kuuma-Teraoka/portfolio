
'''
　apsのNoise...のような論文では二段組でも数式があるのでそこがうまく認識できない。chatgptが普通にテキストを抽出してくれたのでそっちでいいかも。
'''

import sys
import pdfplumber

# 一段組
'''
with pdfplumber.open(sys.argv[1]) as pdf:
    text = "\n".join([page.extract_text() for page in pdf.pages if page.extract_text()])
print(text)
'''


# 二段組
with pdfplumber.open(sys.argv[1]) as pdf:
    for page in pdf.pages:
        words = page.extract_words()
        left_column, right_column = [], [] 
        x_median = page.width / 2
        for word in words:
            x0, x1 = word['x0'], word['x1']
            text = word['text']
            if x1 < x_median:
                left_column.append(text)
            else:
                right_column.append(text)
        left_text = " ".join(left_column)
        right_text = " ".join(right_column)
        print("左カラム:\n", left_text)
        print("\n右カラム:\n", right_text)

