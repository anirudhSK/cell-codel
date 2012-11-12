import sys
import random
if (len(sys.argv) < 3 ) :
  print "Usage :",sys.argv[0]," <sigma in pkts per sec> <duration in sec>"
  exit()
sigma=float(sys.argv[1])
duration=int(sys.argv[2])
RATE_MAX=1250             # in pkts per second
''' rate evolution using Brownian Motion '''
def rate(prev_rate):
  tmp_rate=0
  ''' do repeated sampling till it falls within our range  '''
  while (tmp_rate <=0 or tmp_rate >= RATE_MAX) :
   tmp_rate=prev_rate + random.gauss(0,sigma)
  return tmp_rate
''' schedule generation  '''
prev_rate=0
for i in range (0,duration): # run for 'duration' seconds
  cur_rate=rate(prev_rate)# get rate using Brownian motion
  delta=1000/cur_rate;    # compute pkt delta in ms
  t=i*1000                # convert bin into ms
  while (t < (i+1)*1000) :# iterate over bin
    print int(round(t))
    t=t+delta;
  prev_rate=cur_rate
