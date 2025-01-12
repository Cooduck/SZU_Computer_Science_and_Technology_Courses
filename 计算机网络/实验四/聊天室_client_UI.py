import socket
import threading
import tkinter


root = tkinter.Tk()
root.title('chatting room')
server_ip = 'localhost'
server_port = 12000
client_socket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
client_socket.bind(('',0))
exit_event = threading.Event()

def receive_messages():
    while not exit_event.is_set():
        try:
            data, server = client_socket.recvfrom(4096)
            text_message.insert(tkinter.END, data.decode('utf-8') + '\n',"receive_color")
        except socket.error:
            break

def send():
    send_msg = text_text.get('0.0', tkinter.END).rstrip('\n')
    text_text.delete('0.0', tkinter.END)
    if send_msg.lower() == 'exit':
        exit_event.set()
        client_socket.sendto(bytes(send_msg, 'utf8'), (server_ip, server_port))
    else:
        client_socket.sendto(bytes(send_msg, 'utf8'), (server_ip, server_port))


message_frame = tkinter.Frame(width=540, height=300, bg='white')
text_frame = tkinter.Frame(width=540, height=100)
send_frame = tkinter.Frame(width=540, height=30)

text_message = tkinter.Text(message_frame)
text_text = tkinter.Text(text_frame)
button_send = tkinter.Button(send_frame, text='send', command=send)

message_frame.grid(row=0, column=0, padx=3, pady=6, sticky="nsew")
text_frame.grid(row=1, column=0, padx=3, pady=6, sticky="nsew")
send_frame.grid(row=2, column=0, sticky="nsew")

text_message.grid(sticky="nsew")
text_text.grid(sticky="nsew")
button_send.grid(sticky="nsew")

root.grid_rowconfigure(0, weight=1)  # 使第一行在窗口高度变化时扩展
root.grid_rowconfigure(1, weight=1)  # 使第二行在窗口高度变化时扩展
root.grid_rowconfigure(2, weight=1)  # 使第三行在窗口高度变化时扩展
root.grid_columnconfigure(0, weight=1)  # 使第一列在窗口宽度变化时扩展

text_message.tag_config("receive_color", foreground="blue")
text_message.tag_config("tip_color", foreground="red")

text_message.insert(tkinter.END, "Please input your name to join the chat:" + '\n',"tip_color")

receive_thread = threading.Thread(target=receive_messages)
receive_thread.start()

root.mainloop()
client_socket.close()
