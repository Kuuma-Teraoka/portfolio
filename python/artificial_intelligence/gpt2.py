from transformers import AutoModelForCausalLM, AutoTokenizer

# モデルとトークナイザのロード
model_name = "gpt2"
model = AutoModelForCausalLM.from_pretrained(model_name)
tokenizer = AutoTokenizer.from_pretrained(model_name)

# `pad_token`を設定
if tokenizer.pad_token is None:
    tokenizer.pad_token = tokenizer.eos_token  # `eos_token`を使って`pad_token`を設定

# 入力テキスト
import sys
input_text = sys.argv[1]
inputs = tokenizer(input_text, return_tensors="pt", padding=True)

# 応答生成
output = model.generate(
    inputs.input_ids,
    attention_mask=inputs.attention_mask,  # Attention Maskを設定
    max_length=50,
    num_return_sequences=1,
    pad_token_id=tokenizer.pad_token_id,  # Padトークンを明示的に設定
)

# 出力をデコードして表示
response = tokenizer.decode(output[0], skip_special_tokens=True)
print("モデルの応答:", response)
