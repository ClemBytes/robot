import os

charset = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/"
assert len(charset) == 64

def base64(data):
    bits = ''.join('{0:08b}'.format(byte) for byte in data)
    bits += '0' * ((6 - len(bits)) % 6)
    i = 0
    res = ''
    while i < len(bits):
        six_bits = bits[i:i+6]
        print(f'{six_bits} : {int(six_bits, 2)}')
        res += charset[int(six_bits, 2)]
        i += 6
    res += '=' * ((4 - len(res)) % 4)
    print(res)
    return res


data = ''
with open('./data/favicon-16x16.png', 'rb') as f:
    data = f.read()
base64(data)
assert base64(b'Many hands make light work.') == "TWFueSBoYW5kcyBtYWtlIGxpZ2h0IHdvcmsu"
assert base64(b'Man') == "TWFu"
assert base64(b'Ma') == "TWE="
assert base64(b'M') == "TQ=="
