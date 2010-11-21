from mq2 import *

p = SpawnList()
d = dict()

while p.IsValid():
	s = p.CleanName
	if d.has_key(s):
		d[s] += 1
	else:
		d[s] = 1
	p = p.Next

for q in d.keys():
	if d[q] > 1:
		print "%s: %i" % (q, d[q])