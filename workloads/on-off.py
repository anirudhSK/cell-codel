#! /usr/bin/python
import sys
import httplib
from time import time
from time import sleep
import random
import thread

# Constants
off_mean=300 # 300 ms off time
flow_mean=100000 # 100 KB
time_start = time();

# Command line arguments
if (len(sys.argv) < 5) :
  print "Usage: enter url, duration in secs, persistence, nsrc"
  exit(5)

url      = sys.argv[1]
duration = int(sys.argv[2])
persistent_str = sys.argv[3]
nsrc = int(sys.argv[4])

# Fetch one flow's worth of bytes
def issue_next_get(start, flow_size, conn, is_persistent):
  time_before_get = time()
  if (is_persistent):
    conn.request("GET", "/randomhuge", '', {'Range' : 'bytes='+str(start)+'-'+str(start + flow_size - 1)})
  else :
    conn.request("GET", "/randomhuge", '', {'Range' : 'bytes='+str(start)+'-'+str(start + flow_size - 1), 'Connection' : 'Close'})

  time_after_get  = time()
  resp = conn.getresponse()
  resp.read()
  time_to_response = time()
  syn_fct = int(1000*(time_to_response-time_before_get))
  get_fct = int(1000*(time_to_response-time_after_get))
  return (start+flow_size, syn_fct, get_fct)

# Simulate an ON/OFF source
def on_off_source(thread_name):
  print thread_name
  # Initialization
  global time_start
  current_time = time() - time_start;
  conn = httplib.HTTPConnection(url);
  byte_marker = 0
  is_persistent = (persistent_str == "persistent")
  counter = 1
    
  while (current_time < duration):
    # Sample OFF duration
    off_time = int(random.expovariate(1.0/off_mean))
    print thread_name," switching off for ", off_time," ms"
   
    # Sleep for that long
    sleep(off_time / 1000.0) 
  
    # Sample flow size
    flow_size = int(random.expovariate(1.0/flow_mean))
  
    # Retrieve flow
    (byte_marker, syn_fct, get_fct) = issue_next_get(byte_marker, flow_size, conn, is_persistent)
 
    # print stats
    print thread_name," Flow #", counter," size: ", flow_size," bytes syn_fct: ", syn_fct," get_fct: ", get_fct 

    # Update time
    current_time = time() - time_start;
    
    # Increment counter
    counter += 1

if __name__ == "__main__" :
  for i in range(0, nsrc):    
    thread.start_new_thread(on_off_source, ("Thread-"+str(i),))
  sleep(duration+1)
