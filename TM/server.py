import socket
import json 
import sys
import selectors
import http

verbose =  True
# Define socket host and port
host = ''
HTTP_PORT = 9001
QB_PORT = 9002
server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
server_socket.bind((host, HTTP_PORT))
server_socket.listen()

sel = selectors.DefaultSelector()
sel.register(server_socket, selectors.EVENT_READ, data=None)
print(f'Server listening on {host}:{HTTP_PORT}...')

def verify_user(username,password):
    f = open('users.json')
    data = json.load(f)

    if username in data and data[username] == password:
        return True
    else:
        return False

def accept(sock):
    conn, addr = sock.accept()  # Should be ready
    if verbose: print('accepted', conn, 'from', addr)

    events = selectors.EVENT_READ | selectors.EVENT_WRITE

    sel.register(conn, events, data=data)
    conn.setblocking(False)
    data = addr

def service_connection(key, mask):
    sock = key.fileobj
    data = key.data
    serve_standard = False
    incorrectlogin = False
    question = False

    if mask & selectors.EVENT_READ:

        recv_data = sock.recv(1024)
        print(recv_data.decode())
        response = recv_data.decode().split('\n')
        rq_type = response[0]
        print(rq_type[4:10])
        if rq_type[0:3] == "GET" and rq_type[4:6] == "/ ":
            serve_standard = True
        elif rq_type[0:3] == "GET" and rq_type[4:10] == "/login":
            creds = (rq_type.split(' ')[1])
            user = creds.split('=')[1].split('&')[0]
            password = creds.split('=')[2]
            verify = verify_user(user,password)
            print(verify)
            if verify:
                question = True
            else:
                incorrectlogin = True


            print(user + " " + password)
            print("login attempted")
                
    
    if mask & selectors.EVENT_WRITE:
        
        if serve_standard:
            response = 'HTTP/1.0 200 OK\n\n' + loginHTML    
            sock.send(response.encode())
        elif incorrectlogin:
            response = 'HTTP/1.0 200 OK\n\n' + loginHTML    
            sock.send(response.encode())
        elif question:
            response = 'HTTP/1.0 200 OK\n\n' + multiHTML
            sock.send(response.encode())
        sel.unregister(sock)
        sock.close()
    else:
        print(f"Closing connection to {data}")
        sel.unregister(sock)
        sock.close()

with open("login_page.html","r") as login_page:
    loginHTML = login_page.read()

with open("multi_choice.html","r") as multi_page:
    multiHTML = multi_page.read()

while True:
    events = sel.select(timeout=None)
    for key,mask in events:
        if key.data is None:
            accept(key.fileobj)
        else:
            service_connection(key,mask)
 
    # Wait for client connections
    #client_connection, client_address = server_socket.accept()
    # Get the client request
    #request = client_connection.recv(1024).decode()
    #print("request is ",request)

    # Send HTTP response
    #response = 'HTTP/1.0 200 OK\n\n' + content
    #client_connection.sendall(response.encode())
    #client_connection.close()

# Clos socket
#server_socket.close()
