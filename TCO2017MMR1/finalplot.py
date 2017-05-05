f = open('finalscore.txt')
lines = f.readlines();
f.close()

import numpy as np
import matplotlib.pyplot as plt

V = []
E = []
Score = []
AllScore=0
for line1,line2 in zip(lines[1::2],lines[2::2]):
    words1 = line1.split()
    words2 = line2.split()

    Score.append(float(words1[1]))
    V.append(int(words2[0]))
    E.append(int(words2[1]))
    AllScore += float(words1[1])
    
AllScore = AllScore/len(Score)
VE = [E/V for E,V in zip(E,V)]
Ave = [[] for i in range(11)]
for i in range(len(Score)):
    Ave[int(round(VE[i]))].append(Score[i])
AveScore = []
start=0
for i in range(11):
    if len(Ave[i])==0:
        start+=1
        continue
    score = 0
    for ave in Ave[i]:
        score+=ave
    score /= len(Ave[i])
    AveScore.append(score)
plt.plot(np.arange(start, 11, 1), np.array(AveScore), label='Score')
plt.plot(np.arange(0, 22, 11), np.array([AllScore,AllScore]),label='AverageScore=' + '{:.5f}'.format(AllScore))
plt.xlim(0.5,10.5)
plt.xticks(np.arange(1,11,1))
plt.legend(loc='best')
#plt.ylabel('Score')
plt.xlabel('NE / NV')
plt.savefig('finalplot1.png')
plt.show()

#V = V[:len(Score):7]
#E = E[:len(Score):7]
#VE = VE[:len(Score):7]
#Score = Score[::7]

x = np.array(VE)
#plt.xlim(10, 1000)
y = np.array(Score)
plt.ylim(0.5,1.02)

plt.ylabel('Score')
plt.xlabel('NE / NV')

plt.plot(x,y, 'o', markeredgecolor='k')

plt.savefig('finalplot.png')

plt.show()
