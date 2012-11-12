import sys
fh=sys.stdin
acc=[0]*int(sys.argv[1]) # get duration from cmd line
for line in fh.readlines() :
  time=int(float(line.split()[0]));
  acc[(time/1000)]=acc[(time/1000)]+8*1500; # ms to sec
for i in range(0,len(acc)) :
    print i,"\t",(acc[i]/1.0e6); # in mbps
