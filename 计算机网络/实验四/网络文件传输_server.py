import socket
import  os

def send_file(conn, filename):
    sum = 0
    # 打开文件并读取内容
    with open(filename, 'rb') as file:
        # 读取文件内容
        data = file.read(64)
        while data:
            # 发送文件内容给客户端
            conn.send(data)
            print(f"Send a chunk of {len(data)} bytes.")
            sum += len(data)
            data = file.read(64)
    return sum

if __name__ == "__main__":
    # 创建一个TCP socket
    server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

    # 绑定IP地址和端口
    server_address = ('', 12000)
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
        filename = connection.recv(2048).decode('utf-8')
        print(f"Received a filename \'{filename}\'.")
        if os.path.exists(filename):
            # 如果文件存在，发送文件给客户端
            tot_bytes = send_file(connection, filename)
            print(f"Send a file of {tot_bytes} bytes.")
            print('Done!')
        else:
            # 如果文件不存在，发送错误消息给客户端
            connection.send('The file is not exit!'.encode('utf-8'))
            print('The file is not exit!')
        connection.close()

