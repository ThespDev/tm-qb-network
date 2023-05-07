import socket

host = 'localhost'
port = 8080

s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s.connect((host, port))
s.sendall(b"Hello World")
data = s.recv(4096)
print("Received {data.decode()}")

s.close()
