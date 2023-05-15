import socket
import json 
import sys
import selectors
import pickle


verbose =  True
# Define socket host and port
host = ''
HTTP_PORT = 9001
QB_PORT = 9002

def find_cookie(headers):
    cookie = None
    for header in headers:
        title = header.split(": ")
        if title[0] == "Cookie":
            cookie = title[1]
    return cookie


def backup_data(db):
    with open("backup","wb") as f:
        pickle.dump(db,f,protocol=pickle.HIGHEST_PROTOCOL)

#For emergency crashes
def open_backup():
    try:
        with open("backup","rb") as f:
            backup = pickle.load(f)
    except:
        backup = None
    return backup

def register_user(username,d):
    d[username]

def verify_user(username,password):
    f = open('users.json')
    data = json.load(f)
    if username in data['users'] and data['users'][username]["password"] == password:
        return data["users"][username]["hash"]
    else:
        return False

def accept(sock,mask,db):
    conn, addr = sock.accept()  # Should be ready
    if verbose: print('accepted', conn, 'from', addr)
    events = selectors.EVENT_READ | selectors.EVENT_WRITE
    sel.register(conn, events, data=service_connection)
    conn.setblocking(False)
    

def service_connection(sock, mask,db):
    recv_data = None
    serve_standard = False
    incorrectlogin = False
    custom_webpage = False
    first_login = False
    connection_type = sock.getsockname()[1]
    print("SOCK INFO ",sock.getsockname()[1])
    print("Event Read", (mask & selectors.EVENT_READ))
    print("Event Write",(mask & selectors.EVENT_WRITE))

    if mask & selectors.EVENT_READ:

        recv_data = sock.recv(1024)
        print("RECEIVED DATA \n",recv_data.decode())
        response = recv_data.decode().split('\n')
        request_cookie = find_cookie(response)
        rq_type = response[0]
        if verbose:
            if request_cookie is not None:
                print("COOKIE = " + request_cookie)
            else:
                print("NO COOKIE :c")
        #print(rq_type[4:10])
        #if rq_type == "test": test=True
        if rq_type[0:3] == "GET" and rq_type[4:6] == "/ " and request_cookie is None:
            serve_standard = True
        elif rq_type[0:3] == "GET" and rq_type[4:10] == "/login":
            creds = (rq_type.split(' ')[1])
            user = creds.split('=')[1].split('&')[0]
            password = creds.split('=')[2]
            user_cookie = verify_user(user,password)
            if user_cookie:
                custom_webpage=True
                if request_cookie is None:
                    first_login=True
                if user_cookie not in db:
                    #resgister the user - make function
                    pass
            else:
                incorrectlogin = True


            if verbose: print("USERNAME AND PASSWORD\n" + user + " " + password)
            if verbose: print("login attempted")
    if not recv_data:
        #close connection
        if verbose: print(f'Client disconneccted: {sock.getpeername()}')
        sel.unregister(sock)
        sock.close()
        return           
    
    if mask & selectors.EVENT_WRITE:
        #if test: response = "hello there"; sock.send(response.encode())
        if serve_standard:
            #Set-Cookie:test=number\n
            response = 'HTTP/1.0 200 OK \n\n' + loginHTML    
            sock.send(response.encode())
        elif incorrectlogin:
            response = 'HTTP/1.0 200 OK\n\n' + loginHTML    
            sock.send(response.encode())
        elif custom_webpage:
            if first_login:
                header = f'HTTP/1.0 200 OK\n Set-Cookie:tm-cookie={user_cookie}\n\n' 
            #response = custom webpage function which uses userstate
            response = 'HTTP/1.0 200 OK\n\n' + multiHTML
            sock.send(response.encode())
        if verbose: print(f'Client disconneccted: {sock.getpeername()}')
        sel.unregister(sock)
        sock.close()
    else:
        if verbose: print(f"Closing connection to {sock.getpeername()}")
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
userinfo = open_backup()
if userinfo is None:
    userinfo = {}


with open("login_page.html","r") as login_page:
    loginHTML = login_page.read()

with open("multi_choice.html","r") as multi_page:
    multiHTML = multi_page.read()

while True:
    events = sel.select(timeout=1)
    for key,mask in events:
        if key.data is None:
            accept(key.fileobj,mask,userinfo)
        else:
            callback = key.data
            callback(key.fileobj,mask,userinfo)
 
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
