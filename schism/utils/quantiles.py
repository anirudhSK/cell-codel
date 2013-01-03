#! /usr/bin/python
import sys
fh=sys.stdin
delay_list=[]
for line in fh.readlines() :
	line.strip();
	if len(line.split()) < 10 :
		continue;
	flow_id=int(line.split()[5])
	if flow_id == int(sys.argv[1]) :
		delay=int(line.split()[3].strip(','))
		delay_list.append(delay)
delay_list.sort()
i=0
for i in range(1,100,1) :
	print delay_list[int(i*0.01*len(delay_list))]," ",i*0.01
print>>sys.stderr,"median ",delay_list[int(0.50*len(delay_list))]
print>>sys.stderr,"95th percentile ",delay_list[int(0.95*len(delay_list))]
print>>sys.stderr,"99th percentile ",delay_list[int(0.99*len(delay_list))]
print>>sys.stderr,"max ",delay_list[-1]
print>>sys.stderr,"length,",len(delay_list)
