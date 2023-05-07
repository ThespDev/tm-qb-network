import socket

# Bind the socket to a local address and port
host = "localhost"
port = 8080

# Set up server socket
s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
try:
    s.bind((host, port))
    # Listen for incoming connections
    s.listen(1)
    print("Server is listening on {host}:{port}")
    # Wait for client to connect
    client_socket, address = s.accept()
    try:
        print("Connection established from {address}")
        while True: 
            # Receive the HTTP request from the client
            data = client_socket.recv(4096)
            if not data:
                break
            client_socket.sendall(data)
    finally:
        client_socket.close()
finally:
    s.close()
