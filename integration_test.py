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
    flag = True
    print("------------------------------------------------------------------")
    print("TEST 1 : CSS template")
    print("---------------------")
    r_css = requests.get('http://127.0.0.0:8000/data/template.css')

    with open("data/template.css", "r") as f:
        expected_css = f.read()

    if not check_diff(r_css.text, expected_css):
        print("Problem with CSS file")
        flag = False

    ct = r_css.headers["Content-Type"]
    if ct != "text/css":
        print(f"Wrong Content-Type: {ct} (received) != text/css (expected)")
        flag = False

    cl = int(r_css.headers["Content-Length"])
    lecss = len(expected_css)
    if cl != lecss:
        print(f"Wrong Content-Length: {cl} (received) != {lecss} (expected)")
        flag = False

    if flag:
        print("OK!")
    print("------------------------------------------------------------------")
    return flag

if __name__ == '__main__':
    main()