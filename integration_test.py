import difflib
import requests

def check_diff(received, expected):
    lr = len(received)
    le = len(expected)
    if lr != le:
        print(f"Lengths differ: {lr} (received) != {le} (expected)")
        return False
    flag = True
    for i in range(lr):
        ri = received[i]
        ei = expected[i]
        if ri != ei:
            print(f"Character {i}: {ri} (received) != {ei} (expected)")
            flag = False
    return flag

def main():
    print("------------------------------------------------------------------")
    print("TEST 1 : CSS template")
    print("---------------------")
    flag1 = True
    r_css = requests.get('http://127.0.0.0:8000/data/template.css')

    with open("data/template.css", "r") as f:
        expected_css = f.read()
    
    if r_css.status_code != requests.codes.ok:
        print(f"Bad status code: {r_css.status_code}")
        flag1 = False

    if not check_diff(r_css.text, expected_css):
        print("Problem with CSS file")
        flag1 = False

    ct = r_css.headers["Content-Type"]
    if ct != "text/css":
        print(f"Wrong Content-Type: {ct} (received) != text/css (expected)")
        flag1 = False

    cl = int(r_css.headers["Content-Length"])
    lecss = len(expected_css)
    if cl != lecss:
        print(f"Wrong Content-Length: {cl} (received) != {lecss} (expected)")
        flag1 = False

    if flag1:
        print("OK!")
    print("------------------------------------------------------------------")
    print("TEST 2 : robot PNG image")
    print("------------------------")
    flag2 = True
    r_png = requests.get('http://127.0.0.0:8000/data/robot.png')

    with open("data/robot.png", "rb") as f:
        expected_robot_png = f.read()

    if r_png.status_code != requests.codes.ok:
        print(f"Bad status code: {r_png.status_code}")
        flag2 = False
    
    if not check_diff(r_png.content, expected_robot_png):
        print("Problem with robot PNG file")
        flag2 = False

    ct = r_png.headers["Content-Type"]
    if ct != "image/png":
        print(f"Wrong Content-Type: {ct} (received) != image/png (expected)")
        flag2 = False

    cl = int(r_png.headers["Content-Length"])
    lecss = len(expected_robot_png)
    if cl != lecss:
        print(f"Wrong Content-Length: {cl} (received) != {lecss} (expected)")
        flag2 = False

    if flag2:
        print("OK!")
    print("------------------------------------------------------------------")
    return flag1 and flag2

if __name__ == '__main__':
    main()