import numpy as np
import matplotlib.pyplot as plt

print('filename:', end='')
filename = input()

f = open(filename + '.txt')
lines = f.readlines()
f.close()

attackT = []
Score = []
turn = 0
for line in lines:
    words = line.split()
    if len(words)==0:
        continue
    if words[0]=='attackT':
        attackT.append(int(words[1]))
    elif words[0]=='Score':
        Score.append(float(words[2]))

P_N = len(attackT)//len(Score)
for i in range(len(Score)):
    attackT[P_N*i] = sum(attackT[P_N*i:P_N*(i+1)])/P_N
attackT=attackT[:len(Score)]
plt.plot(attackT,Score,'o',markeredgecolor='k')

plt.savefig(filename + '.png')
plt.show()
        
