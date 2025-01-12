import socket

def receive_file(first_content, conn, filename):
    sum = 0
    # 打开文件并写入内容
    with open(filename, 'wb') as file:
        # 接收文件内容
        file.write(first_content)
        sum += len(first_content)
        data = conn.recv(64)
        while data:
            # 写入文件
            file.write(data)
            print(f'Received a chunk of {len(data)} bytes.')
            sum += len(data)
            data = conn.recv(64)
    return sum

if __name__ == "__main__":
    # 创建一个TCP socket
    client_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    print('A client is running.')
    # 连接服务器
    server_address = ('127.0.0.1', 12000)  # 服务器的IP地址和端口号
    client_socket.connect(server_address)
    client_host, client_port = client_socket.getsockname()
    print(f'The client address:(\'{client_host}\',{client_port})')
    print(f'Connect to {client_socket.getpeername()}.')

    try:
        # 从用户输入获取文件名
        filename = input('Please input a filename to request: ')

        # 发送文件名给服务器
        client_socket.send(filename.encode('utf-8'))
        print(f"Send the filename\'{filename}\' to the server.")

        # 接收服务器的响应
        response = client_socket.recv(1024)
        if response.decode('utf-8') == 'The file is not exit!':
            print('The server hasn\'t the file.')
        else:
            # 接收文件并保存到硬盘
            tot_byte = receive_file(response, client_socket, 'received_' + filename)
            print(f'A file named \'{"received_" + filename}\' of {tot_byte} bytes is saved.' )

    finally:
        # 关闭连接
        client_socket.close()

