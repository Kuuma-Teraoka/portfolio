from transformers import AutoModelForCausalLM, AutoTokenizer

# モデルとトークナイザのロード
model_name = "gpt2"
model = AutoModelForCausalLM.from_pretrained(model_name)
tokenizer = AutoTokenizer.from_pretrained(model_name)

# 入力テキスト
input_text = "おはよう"
input_ids = tokenizer(input_text, return_tensors="pt").input_ids

# 応答生成
output = model.generate(input_ids, max_length=50, num_return_sequences=1)
response = tokenizer.decode(output[0], skip_special_tokens=True)

# 応答の表示
print("モデルの応答:", response)
