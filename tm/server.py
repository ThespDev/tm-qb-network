import socket
import json 
import sys
import selectors
import pickle
from urllib.parse import unquote_plus

csock = None
javasock = None

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

qb_socks = []
qb_requests = []
verify_requests = []
waiting = []
id = 0

def get_questions():
    pass

def create_html(db,user_cookie):
    request = db["users"][user_cookie]["current_q_content"]
    curr_q  = db["users"][user_cookie]["current_q"]
    attempt_num = db["users"][user_cookie]["attempt_num"]
    if attempt_num[curr_q] < 2 or db["users"][user_cookie]["completed"][curr_q]:
        can_submit = False
    else: can_submit = True
    if request is None:
        return None
    question = request[2]
    type = request[1]
    html = "<body>"
    html += f"<h2>{question}<h2>"
    if type == "Cde":
        html += '<form action="/codeanswer" method="GET">'
        html += '<p> Please put the answer below:</p>'
        if curr_q < 5:

            html += '''<textarea id="code" name="code" rows="10" cols="50">
 public class StdntAnswer{

}
</textarea>'''

        if curr_q > 4:
            html += '''<textarea id="code" name="code" rows="10" cols="50">
 #include <stdio.h>

int main(int argc, char *argv[]) {
return 0;
            }
            </textarea>'''
        if can_submit:
            html += '<input type="submit" value="Submit">'
        if not can_submit:
            html += "<h2> Cannot submit question, max attempts achieved or gotten right</h2>"
        html += '</form>'
        html += """
        <form action = "/nextq" method = "get">
        <input type="submit" value="NEXT QUESTION">
        </form>
        """
        html += """
        <form action = "/prevq" method = "get">
        <input type="submit" value="PREV QUESTION">
        </form>
        """
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

    elif type == "MCA":
        index = 1
        html += '<form action="/mcanswer" method="get">'
        for i in range(3,7):
            #Check if response needs to be in number or text for mcq
            html += f'<input type="radio" name="answer" value="{index}"> {request[i]}<br>'
            index += 1
        if can_submit:
            html += '<input type="submit" value="Submit">'
        if not can_submit:
            html += "<h2> Cannot submit question, max attempts achieved or gotten right</h2>"
        html += '</form>'
        html += """
        <form action = "/nextq" method = "get">
        <input type="submit" value="NEXT QUESTION">
        </form>
        """
        html += """
        <form action = "/prevq" method = "get">
        <input type="submit" value="PREV QUESTION">
        </form>
        """
    html += "</body>"
    return html

def build_request(insock,request,cookie,id):
    rq = {"client_sock":insock,"request":request,"qb_sock":None,"cookie":cookie,"id":id,"sent":False, "type":None}
    return rq

def compare_request(rq):
    for requests in verify_requests:
        if rq["client_sock"] == requests["client_sock"] and rq["cookie"] == requests["cookie"]:
            return True
    return False

#used for cookies, content ect
def find_header(headers,value):
    header_val = None
    for header in headers:
        title = header.split(": ")
        if value == "code" or value == "answer": title=header.split('=')
        if title[0] == value:
            header_val = title[1]
    return header_val


def backup_data(db):
    with open("backup","wb") as f:
        pickle.dump(db,f,protocol=pickle.HIGHEST_PROTOCOL)

def convert_id(id):
    if id<10:
        return '0' + str(id)
    else:
        return str(id)

#For emergency crashes
def open_backup():
    try:
        with open("backup","rb") as f:
            backup = pickle.load(f)
    except:
        backup = None
    return backup

#returns Cookie/True
def search_waiting(sock,q):
    for s in q:
        if s[0] == sock:
            return s[1]
    return False

def register_user(username,user_cookie,d):
    d["users"][user_cookie] = {}
    d["users"][user_cookie]["username"] = username
    d["users"][user_cookie]["finished"] = False
    d["users"][user_cookie]["questions"] = []
    d["users"][user_cookie]["completed"] = [False,False,False,False,False,False,False,False,False,False,]
    d["users"][user_cookie]["current_q"] = 0
    #Index of attempt_num array corresponds to attempts for question of same index
    #if >3, ignore
    d["users"][user_cookie]["attempt_num"] = [4,4,4,4,4,4,4,4,4,4]
    d["users"][user_cookie]["score"] = 0
    d["users"][user_cookie]["current_q_content"] = None
    d["users"][user_cookie]["cookie_sent"] = False
    d["users"][user_cookie]["q_type"] = []

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
        qb_socks.append(sock)
        sel.register(conn, events, data=service_qb)

def service_qb(sock,mask,db):
    global csock
    global javasock
    datab = []
    if mask & selectors.EVENT_READ:
        while True:
            try:
                block = sock.recv(1024)
                if len(block) == 0:
                    break
                datab.append(block)
            except BlockingIOError:
                break
        data = b''.join(datab)
        print(data)
        fields = data.decode().split('\n')
        if fields[0] == "c":
            csock = sock
        elif fields[0] == "java":
            javasock = sock
        for rq in verify_requests:
            if rq["qb_sock"] == sock and fields[0] == "MARKING" and fields[-1] == rq["id"]:
                verify_requests.remove(rq)
                if fields[3] == "Correct":
                    current_q_index = db["users"][rq["cookie"]]["current_q"]
                    db["users"][rq["cookie"]]["completed"][current_q_index] = True
                    db["users"][rq["cookie"]]["score"] += (db["users"][rq["cookie"]]["attempt_num"][current_q_index])%4
                    points = db["users"][rq["cookie"]]["attempt_num"][current_q_index]+1
                    waiting.append((rq["client_sock"],rq["cookie"]))
                    print(f"adding {points}")
            elif rq["qb_sock"] == sock and fields[0] == "RAND_Q" and fields[-1] == rq["id"]:
                type = rq["type"]
                verify_requests.remove(rq)
                questions = fields[1]
                test = questions[1:-1].split(',')
                for i in range(len(test)):
                    test[i] = int(test[i])
                #java came first java is first    
                if sock == javasock and not db["users"][rq["cookie"]]["questions"]:
                    db["users"][rq["cookie"]]["questions"] = test
                    if type == "MCA": db["users"][rq["cookie"]]["q_type"] = ["MCA","MCA","MCA","MCA","MCA"]
                    if type == "Cde": db["users"][rq["cookie"]]["q_type"] = ["Cde","Cde","Cde","Cde","Cde"]
                #java came second java is first
                elif sock == javasock and db["users"][rq["cookie"]]["questions"]:
                    db["users"][rq["cookie"]]["questions"] = test + db["users"][rq["cookie"]]["questions"]
                    if type == "MCA": db["users"][rq["cookie"]]["q_type"] = ["MCA","MCA","MCA","MCA","MCA"] + db["users"][rq["cookie"]]["q_type"]
                    if type == "Cde": db["users"][rq["cookie"]]["q_type"] = ["Cde","Cde","Cde","Cde","Cde"] + db["users"][rq["cookie"]]["q_type"]
                #c came first c is second
                elif sock == csock and not db["users"][rq["cookie"]]["questions"]:
                    db["users"][rq["cookie"]]["questions"] = test
                    if type == "MCA": db["users"][rq["cookie"]]["q_type"] = ["MCA","MCA","MCA","MCA","MCA"]
                    if type == "Cde": db["users"][rq["cookie"]]["q_type"] = ["Cde","Cde","Cde","Cde","Cde"]
                #c came second and c is second
                elif sock == csock and db["users"][rq["cookie"]]["questions"]:
                    db["users"][rq["cookie"]]["questions"] = db["users"][rq["cookie"]]["questions"] + test
                    if type == "MCA": db["users"][rq["cookie"]]["q_type"] = db["users"][rq["cookie"]]["q_type"] + ["MCA","MCA","MCA","MCA","MCA"] 
                    if type == "Cde": db["users"][rq["cookie"]]["q_type"] = db["users"][rq["cookie"]]["q_type"] + ["Cde","Cde","Cde","Cde","Cde"]
            
            elif rq["qb_sock"] == sock and fields[0] == "Q_CONTENT" and fields[-1] == rq["id"]:
                verify_requests.remove(rq)    
                db["users"][rq["cookie"]]["current_q_content"] = fields
                    
                




            


            
    if mask & selectors.EVENT_WRITE:
        if qb_requests:
            rq = qb_requests.pop(0)
            if rq["qb_sock"] == sock:
                verify_requests.append(rq)
                sock.send(rq['request'].encode())
            elif rq["qb_sock"] is None:
                rq["qb_sock"] = sock
                verify_requests.append(rq)
  
                sock.send(rq['request'].encode())
            elif rq["qb_sock"] != sock and  rq["qb_sock"] == csock:
                verify_requests.append(rq)
                csock.send(rq['request'].encode())
            elif rq["qb_sock"] != sock and  rq["qb_sock"] == javasock:
                verify_requests.append(rq)
                javasock.send(rq['request'].encode())



        

def send_questions(sock, html_content,cookie,give_cookie,db):
    if html_content is None: return
    else:
        score = db["users"][cookie]["score"]
        if not db["users"][cookie]["cookie_sent"]:
            response = f'HTTP/1.1 200 OK\nSet-Cookie:tm-cookie={cookie}\n\n' + html_content + f"<h2>Current Marks = {score} "
            db["users"][cookie]["cookie_sent"] = True
        else:
            response = 'HTTP/1.1 200 OK\n\n' + html_content + f"<h2>Current Marks = {score} "
    sock.send(response.encode())

def service_connection(sock, mask,db):
    global id
    data = None
    serve_standard = False
    incorrectlogin = False
    custom_webpage = False
    too_many_attempts = False
    give_cookie = False
 
    
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

        if request_cookie is not None:
            user_cookie = request_cookie.split("=")[1][:-1]
        else:   
            give_cookie = True
        if not search_waiting(sock,waiting):
            if rq_type[0:3] == "GET" and rq_type[4:6] == "/ " and request_cookie is None:
                serve_standard = True
            elif rq_type[0:3] == "GET" and rq_type[4:6] == "/ " and request_cookie:
                custom_webpage = True
                if not search_waiting(sock,waiting):
                    waiting.append((sock,request_cookie.split("=")[1][:-1]))
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
                        num = convert_id(5)
                        rq_id1 = convert_id(id)
                        rq_id2 = convert_id(id)
                        request1 = f"RAND_Q\nCde\n{num}\n{rq_id1}\0"
                        request2 = f"RAND_Q\nMCA\n{num}\n{rq_id2}\0"
                        rq1 = build_request(sock,request1,user_cookie,rq_id1)
                        rq1["type"] = "Cde"
                        rq2 = build_request(sock,request2,user_cookie,rq_id2)
                        rq2["type"] = "MCA"
                        if not compare_request(rq1):
                            qb_requests.append(rq1)
                            id += 1
                        if not compare_request(rq2):
                            rq_id2 = convert_id(id)
                            request2 = f"RAND_Q\nMCA\n{num}\n{rq_id2}\0"
                            rq2 = build_request(sock,request2,user_cookie,rq_id2)
                            rq2["type"] = "MCA"
                            qb_requests.append(rq2)

                   
                else:
                    incorrectlogin = True

            elif rq_type[0:3] == "GET" and rq_type[4:10] == "/nextq" and user_cookie:
                db["users"][user_cookie]["current_q"] += 1
                if db["users"][user_cookie]["current_q"] > 9:
                    db["users"][user_cookie]["current_q"] = 0
                db["users"][user_cookie]["current_q_content"] = None
                waiting.append((sock,user_cookie))
                custom_webpage = True
            elif rq_type[0:3] == "GET" and rq_type[4:10] == "/prevq" and user_cookie:
                db["users"][user_cookie]["current_q"] -= 1
                if db["users"][user_cookie]["current_q"] < 0:
                    db["users"][user_cookie]["current_q"]  = 9
                db["users"][user_cookie]["current_q_content"] = None
                waiting.append((sock,user_cookie))
                custom_webpage = True
            elif rq_type[0:3] == "GET" and rq_type[4:15] == "/codeanswer":
                
                question_num = db['users'][user_cookie]["current_q"]
                q_index = db['users'][user_cookie]["current_q"]
                db['users'][user_cookie]["attempt_num"][q_index] -= 1 
                if db['users'][user_cookie]["attempt_num"][q_index] < 1:
                    too_many_attempts = True
                if not too_many_attempts:
                    code = rq_type.split('=')[1].split(' ')[0]
                    if code == None:
                        code = 'failure'
                   
                    decode = unquote_plus(code)
                    rq_id = convert_id(id)
                    request = f"MARKING\n{question_num}\nCde\n{decode}\n{rq_id}\0"
                    rq = build_request(sock,request,user_cookie,rq_id)
                    if question_num < 5:
                        rq["qb_sock"] = javasock
                    else:
                        rq["qb_sock"] = csock
                    if not compare_request(rq):
                        qb_requests.append(rq)
                        id += 1
                    if not search_waiting(sock,waiting):
                        waiting.append((sock,user_cookie))
                    db["users"][user_cookie]["current_q_content"] = None

                    return
            elif rq_type[0:3] == "GET" and rq_type[4:13] == "/mcanswer":
                print("MCQ submitted")
                questions= db['users'][user_cookie]["questions"]
                q_index = db['users'][user_cookie]["current_q"]
                current_q = questions[q_index]
                db['users'][user_cookie]["attempt_num"][q_index] -= 1 
                if db['users'][user_cookie]["attempt_num"][q_index] < 1:
                    too_many_attempts = True
                if not too_many_attempts:
                    answer = rq_type.split('=')[1][0]
                    rq_id = convert_id(id)
                    qnum = convert_id(current_q)
                    request = f"MARKING\nMCA\n{qnum}\n{answer}\n{rq_id}\0"
                    rq = build_request(sock,request,user_cookie,rq_id)
                    if q_index < 5:
                        rq["qb_sock"] = javasock
                    else:
                        rq["qb_sock"] = csock
                    if not compare_request(rq):
                        qb_requests.append(rq)
                        id += 1
                    if not search_waiting(sock,waiting):
                        waiting.append((sock,user_cookie))
                    db["users"][user_cookie]["current_q_content"] = None

                    return


    if not data and not search_waiting(sock,waiting):
        #close connection
        if verbose: print(f'Client disconected (no data): {sock.getpeername()}\n'); print("-----------------------------------------------\n")
        sel.unregister(sock)
        sock.close()
        return           
    
    if mask & selectors.EVENT_WRITE:
        search = search_waiting(sock,waiting)
        if search:
            user_cookie = search
            waiting.remove((sock,user_cookie))
            custom_webpage = True
        if serve_standard:
            response = 'HTTP/1.1 200 OK \n\n' + loginHTML    
            sock.send(response.encode())
        elif incorrectlogin:
            response = 'HTTP/1.1 200 OK\n\n' + loginHTML + "<h1>INCORRECT LOGIN</h1>"   
            sock.send(response.encode())
        elif custom_webpage:
            if too_many_attempts:
                footer = "<h1>TOO MANY ATTEMPTS</h1>"
            if db["users"][user_cookie]["current_q_content"] is None and db["users"][user_cookie]["questions"]:
                current_q_index = db["users"][user_cookie]["current_q"]
                current_q = db["users"][user_cookie]["questions"][current_q_index]
                current_q = convert_id(current_q)
                curr_id = convert_id(id)
                q_type = db["users"][user_cookie]["q_type"][current_q_index]
                request = f"Q_CONTENT\n{q_type}\n{current_q}\n{curr_id}\0"
                rq = build_request(sock,request,user_cookie,curr_id)
                if current_q_index < 5:
                    rq["qb_sock"] = javasock
                else:
                    rq["qb_sock"] = csock
                if not compare_request(rq):
                    qb_requests.append(rq)
                    id += 1

                if not search_waiting(sock,waiting):
                    waiting.append((sock,user_cookie))
            html = create_html(db,user_cookie)
            if html is None:
                if not search_waiting(sock,waiting):
                    waiting.append((sock,user_cookie))
            send_questions(sock,html,user_cookie,give_cookie,db)
        else: sock.send("HTTP/1.1 200 OK\n\n <h1>uhh</h1>".encode())

sel = selectors.DefaultSelector()
sel.register(server_socket, selectors.EVENT_WRITE | selectors.EVENT_READ, data=accept)
sel.register(qb_socket, selectors.EVENT_WRITE | selectors.EVENT_READ, data=accept)
if verbose: print(f'Server listening on {host}:{HTTP_PORT}...')
if verbose: print(f'Server listening on {host}:{QB_PORT}...')
userinfo = open_backup()
if userinfo is None:
    userinfo = {"users":{}}


with open("login_page.html","r") as login_page:
    loginHTML = login_page.read()

while True:
    backup_data(userinfo)

    events = sel.select(timeout=1)
    for key,mask in events:
        if key.data is None:
            accept(key.fileobj,mask,userinfo)
        else:
            callback = key.data
            callback(key.fileobj,mask,userinfo)
 
