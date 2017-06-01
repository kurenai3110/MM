f = open('maxScore_.txt')
lines = f.readlines()
f.close()

f = open('maxScore.txt','w');
for line in lines:
    words = line.split()
    if words[0] == 'maxScore':
        f.write('Score = '+words[2] + '\n')
f.close()
