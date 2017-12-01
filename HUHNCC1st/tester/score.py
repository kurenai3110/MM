print('filename:', end='')
filename = input()
f = open(filename+'.txt')
lines = f.readlines()
f.close()

print('cnt:', end='')
cnt = int(input())

score = []
score2 = []
sum1 = 0.0
sum2 = 0.0
sum3 = 0.0

#cnt = 0
for line in lines:
    words = line.split()

    if words[0]=='[Score]/[Sum]:':
        score.append(float(words[1][0:-1]))
        score2.append(float(words[2]))
        #cnt+=1


f = open('score_'+filename+'.txt', 'w')

for i in range(cnt):
    if i<40:
        sum1 += score[i]
    elif i<90:
        sum2 += score[i]
    elif i<150:
        sum3 += score[i]


f.write('1_Score:' + str(sum1) + '\n')
f.write('2_Score:' + str(sum2) + '\n')
f.write('3_Score:' + str(sum3) + '\n')

f.close()
