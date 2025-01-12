import socket

if __name__ == "__main__":
    # 创建一个TCP socket
    client_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    print('A client is running.')
    # 连接服务器
    server_address = ('localhost', 12000)  # 服务器的IP地址和端口号
    client_socket.connect(server_address)
    client_host, client_port = client_socket.getsockname()
    print(f'The client address:(\'{client_host}\',{client_port})')
    print(f'Connect to {client_socket.getpeername()}.')

    try:
        while True:
            message = input('Send a request:')
            client_socket.send(message.encode('utf-8'))  # 发送消息给服务器

            # 接收服务器的响应
            response = client_socket.recv(1024).decode('utf-8')
            if message == 'Time':
                print(f'Received the current system time on the server: {response}.')
            elif message == 'Exit':
                print(f'Received a response:{response}.')
                break

    finally:
        # 关闭连接
        client_socket.close()
