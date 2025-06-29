import os

charset = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/"
assert len(charset) == 64

def base64(data):
    j = 0
    res = ''
    while j + 2 < len(data):
        res += charset[data[j + 0] >> 2]
        res += charset[((data[j + 0] & 0b00000011) << 4) | (data[j + 1] >> 4)]
        res += charset[((data[j + 1] & 0b00001111) << 2) | (data[j + 2] >> 6)]
        res += charset[data[j + 2] & 0b00111111]
        j += 3

    if len(data) - j == 1:
        res += charset[data[j + 0] >> 2]
        res += charset[((data[j + 0] & 0b00000011) << 4) | (0 >> 4)]
    elif len(data) - j == 2:
        res += charset[data[j + 0] >> 2]
        res += charset[((data[j + 0] & 0b00000011) << 4) | (data[j + 1] >> 4)]
        res += charset[((data[j + 1] & 0b00001111) << 2) | (0 >> 6)]

    res += '=' * ((4 - len(res)) % 4)
    print(res)
    return res


with open('./data/favicon-16x16.png', 'rb') as f:
    data = f.read()
base64(data)
assert base64(b'Many hands make light work.') == "TWFueSBoYW5kcyBtYWtlIGxpZ2h0IHdvcmsu"
assert base64(b'Man') == "TWFu"
assert base64(b'Ma') == "TWE="
assert base64(b'M') == "TQ=="
