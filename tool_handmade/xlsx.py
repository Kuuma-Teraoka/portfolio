import pandas as pd
import sys

def convert_xlsx_to_csv(xlsx_file, csv_file):
    try:
        # Excelファイルを読み込む
        df = pd.read_excel(xlsx_file, engine='openpyxl')
        # CSVファイルとして保存（UTF-8エンコーディング）
        df.to_csv(csv_file, index=False, encoding='utf-8')
        print(f"Successfully converted {xlsx_file} to {csv_file}")
    except Exception as e:
        print(f"An error occurred: {e}")

if __name__ == "__main__":
    if len(sys.argv) != 3:
        print("Usage: python convert_xlsx_to_csv.py <input_xlsx_file> <output_csv_file>")
    else:
        xlsx_file = sys.argv[1]
        csv_file = sys.argv[2]
        convert_xlsx_to_csv(xlsx_file, csv_file)