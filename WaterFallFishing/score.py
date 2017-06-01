print('filename:', end='')
filename = input()
f = open(filename+'.txt')
lines = f.readlines()
f.close()

score = []
ave = 0.0
cnt = 0
for line in lines:
    words = line.split()
    if words[0]=='Score':
        score.append(float(words[2]))

for i in range(100):
    ave += score[i]

f = open('score_'+filename+'.txt', 'w')
f.write('Score:' + str(ave/100))
f.close()
