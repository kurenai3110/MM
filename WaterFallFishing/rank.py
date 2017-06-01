f0 = open('maxScore.txt')
lines0 = f0.readlines()
f0.close()

print('filename:',end='')
filename = input()
f = open(filename+'.txt')
lines = f.readlines()
f.close()

print('length:',end='')
length = int(input())

score0 = []
for line in lines0:
    words = line.split()
    if words[0] == 'Score':
        score0.append(float(words[2]))

score = []
for line in lines:
    words = line.split()
    if words[0] == 'Score':
        score.append(float(words[2]))


allscore0 = 0
allscore = 0
for i in range(length):
    if max(score[i],score0[i])==0:
        continue
    allscore0 += score0[i]/max(score[i],score0[i])
    allscore += score[i]/max(score[i],score0[i])
allscore0*=1000000/length
allscore*=1000000/length
        
f = open('rank_'+filename+'.txt', 'w')
f.write('Score0:' + str(allscore0) + '\n')
f.write('Score:' + str(allscore) + '\n')
f.close()
