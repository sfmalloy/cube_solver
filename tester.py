from subprocess import Popen, PIPE

def single_run(scramble, version, alg, p = 1):
	proc = Popen(['./driver'], stdin=PIPE, stdout=PIPE, stderr=PIPE)
	
	if version == 's':
		params = bytes(f'{scramble}\n{version}\n{alg}', 'utf-8')
	else:
		params = bytes(f'{scramble}\n{version}\n{alg} {p}', 'utf-8')

	output = proc.communicate(input=params)
	time = float(output[0].decode().split('\n')[-2].split()[1])
	
	return time

def multi_run(runs, scramble, version, alg, p = 1):
	times = 0
	for i in range(runs):
		times += single_run(scramble, version, alg, p)
	
	return round(times / 3, 3)

scramble_data = open('scrambles.dat', 'r').readlines()

csv_version = 'threads'

if csv_version == 'length':
	csv = open('vary_size.csv', 'w')
	algs = ['bfs','astar']
	versions = ['s','p']
	for a in algs:
		for v in versions:
			for line in scramble_data:
				l = len(line.split())
				t = multi_run(3, line, v, a, 8)
				write_line = f'{l},{a},{v},{t}\n'
				print(write_line, end='')
				csv.write(write_line)
elif csv_version == 'threads':
	csv = open('vary_p.csv', 'w')
	algs = ['bfs','astar']
	for a in algs:
		for p in range(1, 9):
			for i in range(39,50):
				t = multi_run(3, scramble_data[i], 'p', a, p)
				write_line = f'{i + 1},{a},{p},{t}\n'
				print(write_line, end='')
				csv.write(write_line)

