
import torch
from transformers import AutoModelForCausalLM, AutoTokenizer

model_dir = "/Users/pegasus/inbox/elyza"
offload_folder = "/Users/pegasus/inbox/offload"
print("model loading...")
tokenizer = AutoTokenizer.from_pretrained(model_dir)
model = AutoModelForCausalLM.from_pretrained(
    model_dir,
    #torch_dtype="auto",
    torch_dtype = torch.float16, # メモリ削減
    device_map = "auto",
    offload_folder = offload_folder
)

while True:
    try:
        prompt = input("\ninput: ")
        if prompt.strip() == "":
            continue

        print(f"計算中...")
        
        inputs = tokenizer(prompt, return_tensors="pt").to(model.device) 
        with torch.no_grad():
            output = model.generate(**inputs, max_new_tokens=100, do_sample=True, top_p=0.95, temperature=0.7) 
        response = tokenizer.decode(output[0], skip_special_tokens=True)

        print(f"ELYZA: {response}")

    except KeyboardInterrupt: 
        break

