import socket


# Define socket host and port
host = 'localhost'
port = 8000


with open("Question_Layout.html","r") as login_page:
    content = login_page.read()


# Create socket
server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
server_socket.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
server_socket.bind((host, port))
server_socket.listen(1)
print('Listening on port %s ...' % port)

while True:    
    # Wait for client connections
    client_connection, client_address = server_socket.accept()
    # Get the client request
    request = client_connection.recv(1024).decode()
    print("request is ",request)

    # Send HTTP response
    response = 'HTTP/1.0 200 OK\n\n' + content
    client_connection.sendall(response.encode())
    client_connection.close()

# Clos socket
server_socket.close()
