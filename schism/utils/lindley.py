#! /usr/bin/python
import sys;
fh=open(sys.argv[1],"r");
delays=dict()
for i in range(1,10000):
	delays[i]=[]
for line in fh.readlines() :
	records=line.strip().split();
	if records[0]=="Using" :
		continue;
	if records[0]=="Lindley" :
		delays[int(records[3])] += [int(records[5])]
	if records[0]=="seqnum" :
		delays[int(records[1])] += [int(records[3])]

for seqnum in delays :
	delay_list=delays[seqnum]
	if all( delay == delay_list[0] for delay in delay_list) :
		print "passed seqnum ",seqnum, " delay_list ", delay_list
	else :
		print "Failed seqnum ",seqnum, " delay_list ", delay_list
