f = open('output.txt')
lines = f.readlines()
f.close()

sum = 0
list = [[str(0),i,str(0),str(0),str(0)] for i in range(10001)]
cnt = 0
for line in lines:
    words = line.split()
    if words[0]=='Score':
        list[cnt][0] = words[2]
        sum += float(words[2])
        cnt+=1
    elif words[0] == 'seed':
        list[cnt][4] = words[1]
    elif words[0] == 'Number':
        if words[3] == 'NV':
            list[cnt][2] = words[5]
        elif words[3] == 'NE':
            list[cnt][3] = words[5]

list.sort()

f = open('score2.txt', 'w')
f.write('SCORE:'+str(sum/cnt)+'\n')
for i in range(10001):
    if list[i][0]=='0':
        continue
    f.write(str(list[i][1]+1)+' '+list[i][0] + '\n')
    f.write(list[i][2] + ' ' + list[i][3] + ' ' + list[i][4] + '\n')
f.close()
