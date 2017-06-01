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
    if words[0] == 'seed':
        score.append([])
        cnt+=1
    if words[0]=='Score':
        score[cnt-1].append(float(words[2]))

for i in range(cnt):
    score[i].sort()
    ave += score[i][len(score[i])//5]

f = open('score_'+filename+'.txt', 'w')
f.write('Score:' + str(ave/cnt))
f.close()
