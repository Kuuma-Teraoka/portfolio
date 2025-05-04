'''
　エクセルのxlsxをcsvに変換するコード。そもそもエクセル自体、個人では使う予定はなく、本来であればウェブサイトを自作して、そこから情報を収集すべきである。なので、今後使うことはないかもしれないがメモをしておく。

　xlsxはopenpyxlに、xlsならxlrdを使う。

　sheetごとにファイルに保存するようにした。
'''

import pandas
import sys
import os
import openpyxl



'''
if len(sys.argv) != 3:
    print("Usage: python convert_xlsx_to_csv.py <input_xlsx_file> <output_csv_file>")
else:
    dataframe = pandas.read_excel(sys.argv[1], engine='xlrd', dtype=str)
    dataframe.to_csv(sys.argv[2], index=False, encoding='utf-8')
    print(f"Successfully converted {sys.argv[1]} to {sys.argv[2]}")
'''


def excel_to_csv(input_file, output_dir):
    file_ext = os.path.splitext(input_file)[1].lower()
    os.makedirs(output_dir, exist_ok=True)
    if file_ext == ".xls":
        excel_data = pandas.read_excel(input_file, sheet_name=None, engine='xlrd', dtype=str)
    elif file_ext == ".xlsx":
        excel_data = pandas.read_excel(input_file, sheet_name=None, engine='openpyxl', dtype=str)
    else:
        print('excel_to_csv(): file_ext != ".xls", ".xlsx"')
        return
    for sheet_name, dataframe in excel_data.items():
        output_csv = os.path.join(output_dir, f"{sheet_name}.csv")
        dataframe.to_csv(output_csv, index=False, encoding='utf-8')
        print(f"generated {output_csv}")


# 行幅調整時に、全角と半角で文字あたりの長さが違うので。
def get_effective_length(s):
    length = 0
    for c in s:
        if ord(c) > 255:
            length += 2  # 全角
        else:
            length += 1  # 半角
    return length


def csvs_to_excel(input_dir, output_file):
    writer = pandas.ExcelWriter(output_file, engine='openpyxl')
    for csv_file in os.listdir(input_dir):
        if csv_file.endswith(".csv"):
            sheet_name = os.path.splitext(csv_file)[0]
            csv_path = os.path.join(input_dir, csv_file)
            dataframe = pandas.read_csv(csv_path, dtype=str, engine="python")
            dataframe.to_excel(writer, sheet_name=sheet_name, index=False)
            print(f"added sheet {sheet_name}")
    writer.close()
 
    # --- ここから列幅自動調整＆wrap_text設定 ---
    workbook = openpyxl.load_workbook(output_file)
    for sheet in workbook.worksheets:
        for column_cells in sheet.columns:
            max_length = 0
            column_letter = column_cells[0].column_letter
            for cell in column_cells:
                if cell.value:
                    # 折り返し設定
                    cell.alignment = openpyxl.styles.Alignment(wrap_text=True)
                    # 列幅調整用
                    cell_value_length = get_effective_length(str(cell.value))
                    if cell_value_length > max_length:
                        max_length = cell_value_length
                    # 行高さ調整用
                    num_lines = str(cell.value).count('\n') + 1
                    if sheet.row_dimensions[cell.row].height is None:
                        sheet.row_dimensions[cell.row].height = 15 * num_lines
                    else:
                        sheet.row_dimensions[cell.row].height = max(sheet.row_dimensions[cell.row].height, 15 * num_lines)
            adjusted_width = max_length + 2
            sheet.column_dimensions[column_letter].width = adjusted_width
    workbook.save(output_file)
 
    print(f"generated {output_file}")


if __name__ == "__main__":
    if len(sys.argv) != 3:
        print("Usage: python3 csvs_to_excel.py <input_csv_directory> <output_excel_file>")
        print("Usage: python3 xlsx.py <input_xlsx_or_xls> <output_directory>")
    else:
        if sys.argv[1].endswith(".xlsx"):
            excel_to_csv(sys.argv[1], sys.argv[2])
        elif sys.argv[2].endswith(".xlsx"):
            csvs_to_excel(sys.argv[1], sys.argv[2])

