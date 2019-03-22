def int_sqrt32(x):
    res = 0
    add = 0x8000
    for i in range(16):
        temp = res | add
        g2 = temp * temp
        if x >= g2:
            res = temp
        add = add >> 1
    return res


a = int_sqrt32(9)
print(a)
