import numpy as np
import matplotlib.pyplot as plt

print('filename:', end='')
filename = input()

f = open(filename + '.txt')
lines = f.readlines()
f.close()

henka = [[] for _ in range(2000)]
turn = 0
owner = -1
n = 0
growth = 0
for line in lines:
    words = line.split()
    if len(words)==0:
        continue
    if words[0]=='TURN':
        turn = int(words[1])
    elif words[0]=='Owner':
        owner=int(words[1])
    elif words[0]=='Num':
        if owner==0:
            n=int(words[1])
        else:
            n=int(words[1])
    elif words[0]=='Growth':
        growth=int(words[1])
        henka[turn].append([owner, n, growth])

for i in range(2000):
    if len(henka[i]):
        for info in henka[i]:
            markers=['o','s','x','^','D']
            colors=['r','b','g','c','m']
            plt.plot(i,info[2],marker=markers[info[0]],markersize=info[1]//50 + 1, color = colors[info[0]])

plt.savefig(filename + '.png')
plt.show()
        
