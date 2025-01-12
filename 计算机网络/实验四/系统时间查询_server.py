import socket
from datetime import datetime


if __name__ == "__main__":
    # 创建一个TCP socket
    server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

    # 绑定IP地址和端口
    server_address = ('localhost', 12000)
    server_socket.bind(server_address)

    # 监听连接请求
    server_socket.listen(1)
    print('-'*30)
    print('The server is ready to connect.')
    server_host, server_port = server_socket.getsockname()
    print(f'The server address:(\'{server_host}\' {server_port})')

    while True:
        connection, client_address = server_socket.accept()
        connection_host, connection_port = connection.getsockname()
        print('-' * 30)
        print('Accepted a new connection.')
        print(f'The connection address:(\'{connection_host}\',{connection_port})')
        print(f'The client address:{client_address}')
        while True:
            data = connection.recv(1024).decode('utf-8')
            print(f'Received a request:{data}.')
            if data == 'Time':
                now = datetime.now().strftime('%Y-%m-%d %H:%M:%S')  # 获取当前系统时间
                print(f'Send a response: {now}.')
                connection.send(now.encode('utf-8'))  # 发送系统时间给客户端
            elif data == 'Exit':
                connection.send('Bye'.encode('utf-8'))  # 发送结束消息给客户端
                print(f'Send a response:Bye.')
                break
        connection.close()
