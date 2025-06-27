import os

def base64_perso(path):
    s = ''
    with open(path, 'rb') as f:
        for line in f:
            for c in line:
                s += '{0:08b}'.format(c)
    print(s)
    print('\n')
    i = 0
    while i < len(s) - 6:
        binary = s[i:i+6]
        print(f'{binary} : {int(binary, 2)}')
        i += 6

# base64_perso('./data/favicon-16x16.png')
base64_perso('./test')