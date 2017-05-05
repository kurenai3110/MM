f = open('result.txt')
lines = f.readlines()
f.close()

sum = 0
list = [[str(0),str(0),str(0),str(0), i] for i in range(1000)]
cnt = -1
for line in lines:
    words = line.split()
    if len(words)==0:
        continue
    
    if words[0]=='score':
        cnt+=1
        list[cnt][0] = words[2]
        sum += float(words[2])
    elif words[0] == 'seed':
        list[cnt][3] = words[2]
    elif words[0] == 'Number':
        if words[3] == 'NV':
            list[cnt][1] = words[5]
        elif words[3] == 'NE':
            list[cnt][2] = words[5]

list.sort()

f = open('finalscore.txt', 'w')
f.write('SCORE:'+str(sum/cnt)+'\n')
for i in range(1000):
    #if list[i][0]=='0':
    #    continue
    f.write(str(list[i][4]+1) + ' ' + list[i][0] + '\n')
    f.write(list[i][1] + ' ' + list[i][2] + ' ' + list[i][3] + '\n')
f.close()
