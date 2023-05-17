import socket
import json 
import sys
import selectors
import pickle
from urllib.parse import unquote


verbose =  True
# Define socket host and port
host = ''
HTTP_PORT = 9001
QB_PORT = 9002
server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
server_socket.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)

server_socket.bind((host, HTTP_PORT))
server_socket.listen()

qb_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
qb_socket.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)

qb_socket.bind((host, QB_PORT))
qb_socket.listen() 

qb_requests = []


def create_html(request):
    
    question = request[1]
    type = request[2]
    html = "<body>"
    html += f"<h2>{question}<h2>"
    if type == "c":
        html += '<form action="/codeanswer" method="post">'
        html += '<p> Please put the answer below:</p>'
        html += '<textarea id="code" name="code" rows="10" cols="50"></textarea>'
        html += '<input type="submit" value="Submit">'
        html += '</form>'
        html += """
        <script>
        document.getElementById("code").onkeydown = function(e) {
            if (e.key == 'Tab') {
                e.preventDefault();
                var start = this.selectionStart;
                this.value = this.value.substring(0, start) + "\t" + this.value.substring(this.selectionEnd);
                this.selectionStart = this.selectionEnd = start + 1;
            }
        };
        </script>

        """

    elif type == "m":
        index = 1
        html += '<form action="/mcanswer" method="post">'
        for i in range(3,7):
            #Check if response needs to be in number or text for mcq
            html += f'<input type="radio" name="answer" value="{request[i]}"> {request[i]}<br>'
            index += 1
        html += '<input type="submit" value="Submit">'
        html += '</form>'
    html += "</body>"
    return html

#used for cookies, content ect
def find_header(headers,value):
    header_val = None
    for header in headers:
        title = header.split(": ")
        if value == "code": title=header.split('=')
        if title[0] == value:
            header_val = title[1]
    return header_val


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

def register_user(username,user_cookie,d):
    d["users"][user_cookie] = {}
    d["users"][user_cookie]["username"] = username
    d["users"][user_cookie]["finished"] = False
    d["users"][user_cookie]["questions"] = []
    d["users"][user_cookie]["current_q"] = None
    #Index of attempt_num array corresponds to attempts for question of same index
    #if >3, ignore
    d["users"][user_cookie]["attempt_num"] = [0,0,0,0,0,0,0,0,0,0]
    d["users"][user_cookie]["score"] = 0

def verify_user(username,password):
    with open('users.json') as f:
        data = json.load(f)
    if username in data['users'] and data['users'][username]["password"] == password:
        print("found user and hash: ", data["users"][username]["hash"])
        return data["users"][username]["hash"]
    else:
        return False

def accept(sock,mask,db):
    conn, addr = sock.accept()  # Should be ready
    if verbose: print('accepted', conn, 'from', addr)
    events = selectors.EVENT_READ | selectors.EVENT_WRITE
    conn.setblocking(False)
    if sock == server_socket:
        sel.register(conn, events, data=service_connection)
    elif sock == qb_socket:
        sel.register(conn, events, data=service_qb)

def service_qb(sock,mask,db):
    datab = []
    if mask & selectors.EVENT_READ:
        while True:
            try:
                block = sock.recv(1024)
                print(block)
                if len(block) == 0:
                    print("breaking")
                    break
                datab.append(block)
            except BlockingIOError:
                print("error")
                break
        data = b''.join(datab)
        print(data)
        sock.send("test".encode())
    if mask & selectors.EVENT_WRITE:
        if qb_requests:
            sock.send(qb_requests.pop(0)[1])
        

def send_questions(sock, html_content,cookie,give_cookie):
    if give_cookie: response = f'HTTP/1.1 200 OK\nSet-Cookie:tm-cookie={cookie}\n\n' + html_content
    else: response = 'HTTP/1.1 200 OK\n\n' + html_content
    sock.send(response.encode())

def service_connection(sock, mask,db):
    data = None
    serve_standard = False
    incorrectlogin = False
    custom_webpage = False
    too_many_attempts = False
    #first_login = False
    give_cookie = False
    #connection_type = sock.getsockname()[1]
    print("Event Read", (mask & selectors.EVENT_READ))
    print("Event Write",(mask & selectors.EVENT_WRITE))

    if mask & selectors.EVENT_READ:
        datab = []
        while True:
            try:
                block = sock.recv(1024)
                if len(block) == 0:
                    break
                datab.append(block)
            except BlockingIOError:
                break
        data = b''.join(datab)
        print("RECEIVED DATA \n",data.decode())
        response = data.decode().split('\n')
        request_cookie = find_header(response,"Cookie")
        rq_type = response[0]
        if verbose:
            if request_cookie is not None:
                #print("COOKIE = " + request_cookie)
                user_cookie = request_cookie.split("=")[1][:-1]
            else:
                give_cookie = True
                #print("NO COOKIE :c")
        if rq_type[0:3] == "GET" and rq_type[4:6] == "/ " and request_cookie is None:
            serve_standard = True
        elif rq_type[0:3] == "GET" and rq_type[4:6] == "/ " and request_cookie:
            custom_webpage = True
        elif rq_type[0:3] == "GET" and rq_type[4:10] == "/login":
            creds = (rq_type.split(' ')[1])
            user = creds.split('=')[1].split('&')[0]
            password = creds.split('=')[2]
            user_cookie = verify_user(user,password)
            print("USER LOGGING IN WITH COOKIE", user_cookie)
            if user_cookie:
                custom_webpage=True
                if user_cookie not in db:
                    register_user(user,user_cookie,db)
                    pass
            else:
                incorrectlogin = True
        elif rq_type[0:4] == "POST" and rq_type[5:16] == "/codeanswer":
            print(user_cookie)
            for key in db['users'].keys():
                print("KEY ",key)
                print("COOKIE: ",user_cookie)
                print("EQUALS ",key==user_cookie)
            question_num = db['users'][user_cookie]["current_q"]
            #q_index = db['users'][user_cookie]["questions"].index(question_num)
            q_index = 2
            db['users'][user_cookie]["attempt_num"][q_index] += 1 
            if db['users'][user_cookie]["attempt_num"][q_index] > 3:
                too_many_attempts = True
            if not too_many_attempts:
                code = find_header(response,"code")
                if code == None:
                    code = 'failure'
                #print("CODE =\n")
                #print(code)
                decode = unquote(code)
                question_num = db['users'][user_cookie]
                request = f"MARKING\n{question_num}\nMCA\n{decode}"
                sock.send("HTTP/1.1 200 OK\n\n<h1>Marking result...</h1>".encode())
                qb_requests.append((sock,request))
                print("returning please don't DC")
                return
    if not data:
        #close connection
        if verbose: print(f'Client disconected (no data): {sock.getpeername()}\n'); print("-----------------------------------------------\n")
        sel.unregister(sock)
        sock.close()
        return           
    
    if mask & selectors.EVENT_WRITE:
        #if test: response = "hello there"; sock.send(response.encode())
        if serve_standard:
            response = 'HTTP/1.1 200 OK \n\n' + loginHTML    
            sock.send(response.encode())
        elif incorrectlogin:
            response = 'HTTP/1.1 200 OK\n\n' + loginHTML + "<h1>INCORRECT LOGIN</h1>"   
            sock.send(response.encode())
        elif custom_webpage:
            #if first_login:
            #    print("user cookie is", user_cookie)
            #    header = f'HTTP/1.1 200 OK\nSet-Cookie:tm-cookie={user_cookie}\n\n'
            #    response = header + "<h1>CITS3002 Project</h1>"
            #    sock.send(response.encode())
            #    print("SENDING HTML \n", response)
            if too_many_attempts:
                footer = "<h1>TOO MANY ATTEMPTS</h1>"
            html = create_html(["Q_CONTENT","Question?","c","apples","baananas","cake","pie"])
            send_questions(sock,html,user_cookie,give_cookie)

sel = selectors.DefaultSelector()
sel.register(server_socket, selectors.EVENT_WRITE | selectors.EVENT_READ, data=accept)
sel.register(qb_socket, selectors.EVENT_WRITE | selectors.EVENT_READ, data=accept)
if verbose: print(f'Server listening on {host}:{HTTP_PORT}...')
if verbose: print(f'Server listening on {host}:{QB_PORT}...')
userinfo = open_backup()
if userinfo is None:
    userinfo = {"users":{}}
print("USERINFO:\n")
print(userinfo)

with open("login_page.html","r") as login_page:
    loginHTML = login_page.read()

with open("multi_choice.html","r") as multi_page:
    multiHTML = multi_page.read()

while True:
    backup_data(userinfo)
    events = sel.select(timeout=1)
    for key,mask in events:
        if key.data is None:
            accept(key.fileobj,mask,userinfo)
        else:
            callback = key.data
            callback(key.fileobj,mask,userinfo)
 
