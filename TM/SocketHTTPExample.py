import socket


# Define socket host and port
SERVER_HOST = 'localhost'
SERVER_PORT = 8000


with open("multi_choice.html","r") as login_page:
    content = login_page.read()


# Create socket
server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
server_socket.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
server_socket.bind((SERVER_HOST, SERVER_PORT))
server_socket.listen(1)
print('Listening on port %s ...' % SERVER_PORT)

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
