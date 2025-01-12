from socket import *
from datetime import datetime


def find_value_in_dict(dictionary, value):
    for v in dictionary:
        if v == value:
            return 1
    return 0


def brodcast(msg):
    for add in address:
        serverSocket.sendto(msg.encode('utf-8'), add)
        print(f"Send to {add} --> {msg}")


def work():
    message, clientAddress = serverSocket.recvfrom(4096)
    if find_value_in_dict(address, clientAddress) == 1:
        content = message.decode('utf-8')
        if content == 'exit':
            left_client_name = address[clientAddress]
            brodcast(f"[{datetime.now().strftime('%H:%M')}] {left_client_name} has left the chat!")
            del address[clientAddress]
        else:
            brodcast("[" + datetime.now().strftime('%H:%M') + " " + address[clientAddress] + "] " + content)
    else:
        clientname = message.decode('utf-8')

        context = "[ "
        if len(address) > 0:
            for person in address.values():
                context = context + person + " "
            context = context + "] already in the chat. Let's start your chat."
        else:
            context = "Your are the first join the chat! Let's start your chat."
        serverSocket.sendto(context.encode('utf-8'), clientAddress)
        print(f"Send to {clientAddress} --> {context}")

        address[clientAddress] = clientname
        brodcast(f"[{datetime.now().strftime('%H:%M')}] Welcome {clientname} joined the chat!")

if __name__ == "__main__":
    address = {}
    serverPort = 12000
    serverSocket = socket(AF_INET, SOCK_DGRAM)
    serverSocket.bind(('', serverPort))
    print('The server is ready to receive.')
    while True:
        work()
