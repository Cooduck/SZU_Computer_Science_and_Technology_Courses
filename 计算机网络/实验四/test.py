import requests
import json

url = "http://localhost:11434/api/generate"
data = {
    "model": "llama3",
    "prompt": "Why is the sky blue?",
    "stream": False
}

response = requests.post(url, json=data)

# 将响应内容解析为 JSON 格式
response_json = response.json()

# 提取 "response" 内容
response_text = response_json["response"]

# 输出提取的内容
print(response_text)
