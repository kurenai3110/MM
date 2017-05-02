f = open('output.txt')
lines = f.readlines();
f.close()

import numpy as np
import matplotlib.pyplot as plt

V = []
E = []
Score = []
AllScore=0
for line in lines:
    words = line.split()

    if words[0] == 'Number':
        if words[3] == 'NV':
            V.append(int(words[5]))
        elif words[3] == 'NE':
            E.append(int(words[5]))
    elif words[0] == 'Score':
        Score.append(float(words[2]))
        AllScore += float(words[2])
AllScore = AllScore/len(Score)
VE = [E/V for E,V in zip(E,V)]
Ave = [[] for i in range(101)]
for i in range(len(Score)):
    Ave[int(round(VE[i]*10))].append(Score[i])
AveScore = []
start=0
for i in range(101):
    if len(Ave[i])==0:
        start+=0.1
        continue
    score = 0
    for ave in Ave[i]:
        score+=ave
    score /= len(Ave[i])
    AveScore.append(score)
plt.plot(np.arange(start, 10.1, 0.1), np.array(AveScore), label='Score')
plt.plot(np.arange(0, 22, 11), np.array([AllScore,AllScore]),label='AverageScore=' + '{:.2f}'.format(AllScore))
plt.xlim(0.5,10.5)
plt.xticks(np.arange(1,11,1))
plt.legend(loc='best')
#plt.ylabel('Score')
plt.xlabel('NE / NV')
plt.savefig('plot1.png')
plt.show()

V = V[:len(Score):7]
E = E[:len(Score):7]
VE = VE[:len(Score):7]
Score = Score[::7]

x = np.array(VE)
#plt.xlim(10, 1000)
y = np.array(Score)
plt.ylim(0.5,1.02)

plt.ylabel('Score')
plt.xlabel('NE / NV')

plt.plot(x,y, 'o', markeredgecolor='k')

plt.savefig('plot.png')

plt.show()
