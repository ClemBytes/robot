import socket

# Create socket
s = socket.socket()

# Connect to localhost
s.connect(("127.0.0.0", 8000))

print("-------------------")
print("1) Request CSS file")
# Send request for CSS
s.sendall(b"GET /data/template.css HTTP/1.1\r\n\r\n")
with open("data/template.css") as f:
    css = f.read()

css_expected = f"HTTP/1.0 200 OK\r\nContent-Type: text/css\r\nContent-Length: {len(css)}\r\n\r\n{css}"

# Receive answer
data = s.recv(10000)

if data.decode() == css_expected:
    print("OK for CSS request")
else:
    print("ERROR for CSS request:")
    print(f"Expected:")
    print("----")
    print(css_expected)
    print("----")
    print(f"Received:")
    print("----")
    print(data.decode())
    print("----")

print("-------------------")

print("\n")
print("-------------------")
print("2) Request PNG file")
print("-------------------")
# Send request for CSS
s.sendall(b"GET /data/robot.png HTTP/1.1\r\n\r\n")

# Receive answer
data = s.recv(10000)

# print(data)
print("-------------------")

s.close()