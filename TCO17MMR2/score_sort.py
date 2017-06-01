print('filename:', end='')
filename = input()
f = open(filename+'.txt')
lines = f.readlines()
f.close()

sc = []
cnt = 0
for line in lines:
    words = line.split()
    if words[0]=='Score':
        sc.append([float(words[2]),cnt+1])
        cnt+=1

sc.sort()

f = open('score_list_'+filename+'.txt', 'w')
for s in sc:    
    f.write('{0:>3}'.format(s[1]) + ' ')
    f.write('Score : ' + '{0:10.6f}'.format(s[0]) + '\n')
f.close()
