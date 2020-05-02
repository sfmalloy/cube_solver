old = open('scrambles_old.dat', 'r')
new = open('scrambles.dat', 'w')

l = 1
count = 0
for line in old.readlines():
	line = line.split('.')[1].split()
	if count == 10:
		l += 1
		count = 0
	new.write(' '.join(line[0:l]) + '\n')
	count += 1
