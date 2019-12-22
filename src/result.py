import matplotlib.pyplot as plt

maxcnum = 11
r = 2000
fin = open("./"+str(maxcnum)+"_modu_check_result.txt", "r");
d = []
while 1:
    line = fin.readline()
    if line == "":
        break
    val = line.split()
    val1 = [int(i) for i in val[1:]]
    d.append(val1)

out = {}
for i in d:
    if not i[0] in out:
        out[i[0]] = [0, 0]
    if i[1] == 1:
        out[i[0]][0] += 1
    elif i[1] == 0:
        out[i[0]][1] += 1

print(out)
