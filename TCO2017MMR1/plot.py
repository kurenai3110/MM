f = open('output.txt')
lines = f.readlines();
f.close()

import numpy as np
import matplotlib.pyplot as plt

V = []
E = []
Score = []

for line in lines:
    words = line.split()

    if words[0] == 'Number':
        if words[3] == 'NV':
            V.append(int(words[5]))
        elif words[3] == 'NE':
            E.append(int(words[5]))
    elif words[0] == 'Score':
        Score.append(float(words[2]))

VE = [E/V for E,V in zip(E,V)]

V = V[::10]
E = E[::10]
VE = VE[::10]
Score = Score[::10]

x = np.array(VE)
#plt.xlim(10, 1000)
y = np.array(Score)
#plt.ylim(-0.1,1.1)

plt.ylabel('Score')
plt.xlabel('NE / NV')

plt.plot(x,y, 'o')

plt.savefig('plot.png')

plt.show()
