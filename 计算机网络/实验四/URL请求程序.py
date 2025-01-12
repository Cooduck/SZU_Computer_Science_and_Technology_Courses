import requests

def download_and_save_html(url, filename):
    # 发送GET请求
    response = requests.get(url)
    # 将响应内容保存到文件
    with open(filename, 'w', encoding= "utf-8") as file:
        file.write(response.text)
    # 返回响应内容的大小
    return len(response.text)

if __name__ == "__main__":
    url = input("Please input a URL:")
    filename = f"{url.split('//')[-1].split('/')[0]}.html"  # 存储的文件名

    # 下载并保存网页，并获取文件大小
    file_size = download_and_save_html(url, filename)

    # 打印所请求网页的URL、存储文件名、文件大小等信息
    print("URL:", url)
    print("save_file_name:", filename)
    print("save_file_size:", file_size, "bytes")
