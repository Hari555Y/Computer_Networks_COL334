import threading
import socket
import re
#singleclient-server mode
import socket
ip = "127.0.0.1"
port = 2456
server = socket.socket(socket.AF_INET,socket.SOCK_STREAM)
server.bind((ip,port))
server.listen()
while True:
    client,address = server.accept()
    print(f'connected {address}')
    msg = client.recv(1024)
    codes = msg.decode('utf-8')
    print(codes)
    client.send(codes.encode('utf-8'))
    client.close()

#multiclient broadcasting model

import threading
import socket
import re
host = '127.0.0.1'
port = 50390
server = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
server.bind((host,port))
server.listen()
clients = []
client_map ={}

def broadcast(message, client):
    for c in clients:
        if c!=client:
            c.send(message)



def handle(client):
    while True:
        try:
            message = client.recv(1024)
            broadcast(message,client)
        except:
            continue


def recieve():
    while True:
        client,address = server.accept()
        print(f'connection established with {str(address)}')
        client.send('name'.encode('utf-8'))
        names = client.recv(1024).decode('ascii')
        clients.append(client)
        client_map[names] = client
        print(f'REGISTER TOSEND {names}')
        print(f'REGISTER TORECV {names}')
        thread = threading.Thread(target =handle , args = (client, ))
        thread.start()


print('Waiting for connections......')
recieve()



# single server multicast broadcasting and unicasting 
import threading
import socket
host = '127.0.0.1'
port = 59000
server = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
server.bind((host,port))
server.listen(4)
clients = []
clients_map = {}

def broadcast(message, client):
    for c in clients:
        if c!=client:
            c.send(message)

def unicast(message, torecvr):
    if clients.has_key(torecvr):
        clintsend = clients_map[torecvr]
        clintsend.send((message))

    else:
        print('NO USER REGISTERED')


def handle(client):
    while True:
        try:
            messagesss = client.recv(1024).decode('utf-8')
            list = messagesss.split(' ', 1)
            if (list[0][1:len(list[0])] == 'ALL'):
                broadcast(list[1].encode('utf-8'), client)
            else:
                unicast(list[1].encode('utf-8'), list[0][1:len(list[0])])
        except:
            continue

def recieve():
    while True:
        client,address = server.accept()
        print(f'connection established with {str(address)}')
        client.send('name'.encode('utf-8'))
        names = client.recv(1024).decode('ascii')
        clients.append(client)
        clients_map[names] = client
        print(f'REGISTER TOSEND {names}')
        print(f'REGISTER TORECV {names}')
        thread = threading.Thread(target =handle , args = (client, ))
        thread.start()

print('Waiting for connections......')
recieve()















