#! /usr/bin/python
import sys
fh=sys.stdin
delay_dict=dict()
cdf=dict()
for line in fh :
	line.strip();
	if len(line.split()) < 10 :
		continue;
	flow_id=int(line.split()[5])
	if flow_id == int(sys.argv[1]) :
		delay=int(line.split()[3].strip(','))
		if delay in delay_dict :
			delay_dict[delay] += 1
		else :
			delay_dict[delay] = 1
delay_list = delay_dict.keys()
delay_list.sort() 

acc = 0;
total_length = float( sum( delay_dict.values() ) )
for delay in delay_list :
	acc += delay_dict[delay]
	cdf[delay] = acc/total_length ;

for delay in delay_list :
	print delay," ",cdf[delay]

print>>sys.stderr,"median ",[ delay for delay in delay_list if cdf[delay] >= 0.5 ][0];
print>>sys.stderr,"95th percentile ",[ delay for delay in delay_list if cdf[delay] >= 0.95 ][0];
print>>sys.stderr,"99th percentile ",[ delay for delay in delay_list if cdf[delay] >= 0.99 ][0];
print>>sys.stderr,"max ",delay_list[-1];
print>>sys.stderr,"length,",total_length
