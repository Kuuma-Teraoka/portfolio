
# 特定のページだけ90度回転させる。

import sys
from pypdf import PdfReader, PdfWriter

def rotate_selected_pages(input_pdf, output_pdf, pages_to_rotate, rotation=90):
    # PDFを読み込む
    reader = PdfReader(input_pdf)
    writer = PdfWriter()

    # 各ページを処理
    for i, page in enumerate(reader.pages):
        if (i + 1) in pages_to_rotate:  # ページ番号は1から始まる
            page.rotate(rotation)  # 指定した角度で回転
        writer.add_page(page)

    # 新しいPDFを保存
    with open(output_pdf, "wb") as f:
        writer.write(f)

# 使用例
input_pdf = sys.argv[1]  # 入力PDF
output_pdf = sys.argv[2]  # 出力PDF
pages_to_rotate = [24,36,37]  # 回転させたいページ（1始まり）
rotation_angle = 270  # 90度回転（時計回り）

rotate_selected_pages(input_pdf, output_pdf, pages_to_rotate, rotation_angle)
print("回転済みPDFを作成しました:", output_pdf)

