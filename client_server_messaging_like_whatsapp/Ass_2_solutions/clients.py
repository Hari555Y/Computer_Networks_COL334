import threading
import socket
import re
import sys
#single server client model
import socket
ip = "127.0.0.1"
port = 2456
server = socket.socket(socket.AF_INET,socket.SOCK_STREAM)
server.connect((ip,port))
msg = input('NAME: ')
server.send(msg.encode('utf-8'))
rcvr = server.recv(1024)
rcvrcode = rcvr.decode('utf-8')
print(f'server : {rcvrcode}')


#single server multi client broadcast model
import threading
import socket
import re
import sys
name = input("Enter username: " )
pattern = "^[A-Za-z0-9]*$"
state = bool(re.match(pattern, name))
if not state:
    print('ERROR 100: Malicious Username');
    sys.exit()
client = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
client.connect(('127.0.0.1',50390))

def client_receive():
    while True:
        try:
            message = client.recv(1024).decode('utf-8')
            if message == 'name':
                client.send(name.encode('utf-8'))
                print(f'REGISTERED TOSEND {name}')
                print(f'REGISTERED TORECV {name}')
            else:
                print(f'{message}')
        except:
            print('Error')
            client.close()
            break

def client_send():
    while True:
        message = input(f'{name}:')
        client.send(message.encode('utf-8'))

receive_thread = threading.Thread(target= client_receive)
receive_thread.start()

send_thread  = threading.Thread(target = client_send)
send_thread.start()



#singleserver broadcasting and unicasting
import threading
import socket
import re
import sys
name = input("Enter username: " )
pattern = "^[A-Za-z0-9]*$"
state = bool(re.match(pattern, name))
if not state:
    print('ERROR 100: Malicious Username');
    sys.exit()
client = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
client.connect(('127.0.0.1',50390))

def client_receive():
    while True:
        try:
            message = client.recv(1024).decode('utf-8')
            if message == 'name':
                client.send(name.encode('utf-8'))
                print(f'REGISTERED TOSEND {name}')
                print(f'REGISTERED TORECV {name}')
            else:
                print(f'{message}')
        except:
            print('Error')
            client.close()
            break

def client_send():
    while True:
        message = input(f'{name}:')
        client.send(message.encode('utf-8'))

receive_thread = threading.Thread(target= client_receive)
receive_thread.start()

send_thread  = threading.Thread(target = client_send)
send_thread.start()







