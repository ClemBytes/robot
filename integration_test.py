import socket

def diff_bytes(a, b):
    min_len = min(len(a), len(b))
    if len(a) != len(b):
        print(f"Lengths differ: {len(a)} != {len(b)}")
        return
    for i in range(min_len):
        if a[i] != b[i]:
            print(f"Byte {i}: {a[i]:02x} != {b[i]:02x}")

# Create socket
s = socket.socket()

# Connect to localhost
s.connect(("127.0.0.0", 8000))

print("-------------------------")
print("1) Request CSS file")
# Send request for CSS
s.sendall(b"GET /data/template.css HTTP/1.1\r\n\r\n")
with open("data/template.css", "r") as f:
    css = f.read()

css_expected = f"HTTP/1.0 200 OK\r\nContent-Type: text/css\r\nContent-Length: {len(css)}\r\n\r\n{css}"

# Receive answer
data_css = s.recv(10000)

if data_css.decode() == css_expected:
    print("OK for CSS request")
else:
    print("ERROR for CSS request:")
    print(f"Expected:")
    print("----")
    print(css_expected)
    print("----")
    print(f"Received:")
    print("----")
    print(data_css.decode())
    print("----")

print("-------------------------")

print("-------------------------")
print("2) Request robot PNG file")
# Send request for robot image
s.sendall(b"GET /data/robot.png HTTP/1.1\r\n\r\n")
with open("data/robot.png", "rb") as f:
    robot_image = f.read()

robot_image_expected = f"HTTP/1.0 200 OK\r\nContent-Type: image/png\r\nContent-Length: {len(robot_image)}\r\n\r\n".encode() + robot_image

# Receive answer
data_robot_image = s.recv(30000)

if data_robot_image == robot_image_expected:
    print("OK for robot PNG request")
else:
    print("ERROR for robot image request:")
    diff_bytes(data_robot_image, robot_image_expected)

print("-------------------")

s.close()