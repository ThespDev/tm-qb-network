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

userinfo = {}

def register_user(username):
    userinfo[username]

def verify_user(username,password):
    f = open('users.json')
    data = json.load(f)

    if username in data and data[username] == password:
        return True
    else:
        return False

def accept(sock,mask):
    conn, addr = sock.accept()  # Should be ready
    if verbose: print('accepted', conn, 'from', addr)
    events = selectors.EVENT_READ | selectors.EVENT_WRITE
    sel.register(conn, events, data=service_connection)
    conn.setblocking(False)
    

def service_connection(sock, mask):
    recv_data = None
    serve_standard = False
    incorrectlogin = False
    question = False
    test = False
    connection_type = sock.getsockname()[1]
    print("SOCK INFO ",sock.getsockname()[1])
    print("Event Read", (mask & selectors.EVENT_READ))
    print("Event Write",(mask & selectors.EVENT_WRITE))

    if mask & selectors.EVENT_READ:

        recv_data = sock.recv(1024)
        print("RECEIVED DATA \n",recv_data.decode())
        response = recv_data.decode().split('\n')
        rq_type = response[0]
        #print(rq_type[4:10])
        #if rq_type == "test": test=True
        if rq_type[0:3] == "GET" and rq_type[4:6] == "/ ":
            serve_standard = True
        elif rq_type[0:3] == "GET" and rq_type[4:10] == "/login":
            creds = (rq_type.split(' ')[1])
            user = creds.split('=')[1].split('&')[0]
            password = creds.split('=')[2]
            verify = verify_user(user,password)
            if verify:
                question = True
            else:
                incorrectlogin = True


            print("USERNAME AND PASSWORD\n" + user + " " + password)
            print("login attempted")
    if not recv_data:
        #close connection
        if verbose: print(f'Client disconneccted: {sock.getpeername()}')
        sel.unregister(sock)
        sock.close()
        return           
    
    if mask & selectors.EVENT_WRITE:
        #if test: response = "hello there"; sock.send(response.encode())
        if serve_standard:
            response = 'HTTP/1.0 200 OK\n\n' + loginHTML    
            sock.send(response.encode())
        elif incorrectlogin:
            response = 'HTTP/1.0 200 OK\n\n' + loginHTML    
            sock.send(response.encode())
        elif question:
            response = 'HTTP/1.0 200 OK\n\n' + multiHTML
            sock.send(response.encode())
        if verbose: print(f'Client disconneccted: {sock.getpeername()}')
        sel.unregister(sock)
        sock.close()
    else:
        if verbose: print(f"Closing connection to {data}")
        sel.unregister(sock)
        sock.close()



server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
server_socket.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)

server_socket.bind((host, HTTP_PORT))
server_socket.listen()

qb_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
qb_socket.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)

qb_socket.bind((host, QB_PORT))
qb_socket.listen() 

sel = selectors.DefaultSelector()
sel.register(server_socket, selectors.EVENT_WRITE | selectors.EVENT_READ, data=accept)
sel.register(qb_socket, selectors.EVENT_WRITE | selectors.EVENT_READ, data=accept)
if verbose: print(f'Server listening on {host}:{HTTP_PORT}...')
if verbose: print(f'Server listening on {host}:{QB_PORT}...')



with open("login_page.html","r") as login_page:
    loginHTML = login_page.read()

with open("multi_choice.html","r") as multi_page:
    multiHTML = multi_page.read()

while True:
    events = sel.select(timeout=1)
    for key,mask in events:
        if key.data is None:
            accept(key.fileobj)
        else:
            callback = key.data
            callback(key.fileobj,mask)
 
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
