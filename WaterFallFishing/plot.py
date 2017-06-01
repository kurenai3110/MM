f = open('maxScore.txt')
maxlines = f.readlines()
f.close()

print('filename:',end='')
file1 = input()
f = open(file1 + '.txt')
lines1 = f.readlines()
f.close()

print('filename:',end='')
file2 = input()
f = open(file2 + '.txt')
lines2 = f.readlines()
f.close()

maxScore = []
Score1 = []
Score2 = []
for line in lines1:
    words = line.split()
    if words[0]=='Score':
        Score1.append(float(words[2]))

for line in lines2:
    words = line.split()
    if words[0]=='Score':
        Score2.append(float(words[2]))
        
for line in maxlines:
    words = line.split()
    if words[0]=='Score':
        maxScore.append(float(words[2]))


import matplotlib.pyplot as plt

sa = 0
for i in range(1000):
    if Score1[i]>Score2[i]:
        sa+=1
    elif Score1[i]<Score2[i]:
        sa-=1
print(sa)
ratio1 = [Score1[i]-Score2[i] for i in range(1000)]
#ratio2 = [Score2[i]/maxScore[i] for i in range(1000)]
plt.plot(range(1000),ratio1)
#plt.plot(range(1000),ratio2)

plt.savefig('plot_'+file1+'_'+file2+'.png')
plt.show()
