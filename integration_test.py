import socket

# Create socket
s = socket.socket()

# Connect to localhost
s.connect(("127.0.0.0", 8000))

print("-------------------")
print("1) Request CSS file")
print("-------------------")
# Send request for CSS
s.sendall(b"GET /data/template.css HTTP/1.1\r\n\r\n")

# Receive answer
data = s.recv(10000)

print(data.decode())

print("-------------------")

print("\n")
print("-------------------")
print("2) Request PNG file")
print("-------------------")
# Send request for CSS
s.sendall(b"GET /data/robot.png HTTP/1.1\r\n\r\n")

# Receive answer
data = s.recv(10000)

print(data)
print("-------------------")

s.close()