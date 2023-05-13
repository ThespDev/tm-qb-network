import socket

host = 'localhost'
port = 9001

s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s.connect((host, port))
s.sendall("test".encode())
data = s.recv(1024)
print(f"Received {data.decode()}")
s.close()
