import socket
import threading

# 创建事件标志，用于通知线程结束
exit_event = threading.Event()


def receive_messages():
    while not exit_event.is_set():
        try:
            data, server = client_socket.recvfrom(4096)
            print(data.decode('utf-8'))
        except socket.error:
            break


if __name__ == '__main__':

    server_ip = 'localhost'
    server_port = 12000
    client_socket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

    name = input("Please input your name to join the chat:")
    client_socket.sendto(name.encode('utf-8'), (server_ip, server_port))

    receive_thread = threading.Thread(target=receive_messages)
    receive_thread.start()

    while True:
        message = input()
        if message.lower() == 'exit':
            exit_event.set()
            client_socket.sendto(message.encode('utf-8'), (server_ip, server_port))
            receive_thread.join()
            print("<quit>")
            break
        client_socket.sendto(message.encode('utf-8'), (server_ip, server_port))

    client_socket.close()