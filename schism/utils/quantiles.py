#! /usr/bin/python
import sys

# compute complementary CDF
def get_ccdf( threshold, cdf ) :
	if threshold in cdf :
		return 1 - cdf[threshold]
	else :
		closest = min((abs(threshold - delay),delay) for delay in cdf.keys())[1]
		return 1 - cdf[closest]

# file handling
fh=sys.stdin
delay_dict=dict()
cdf=dict()
for line in fh :
	line.strip();
	if ( line.split()[0] == "Using" ) :
		continue
	if len(line.split()) < 10 :
		continue;
	flow_id=int(line.split()[5])
	if flow_id not in delay_dict :
		delay_dict[flow_id] = dict()
	delay=int(line.split()[3].strip(','))
	if delay in delay_dict[ flow_id ] :
		delay_dict[flow_id][delay] += 1
	else :
		delay_dict[flow_id][delay] = 1

# sort delays to get CDF
delay_list=dict()
for flow_id in delay_dict :
	delay_list[flow_id] = delay_dict[flow_id].keys()
	delay_list[flow_id].sort()

# now get CDF for each flow
total_length = dict()
for flow_id in delay_dict :
	total_length[ flow_id ] = float( sum( delay_dict[flow_id].values() ) )
	cdf[flow_id] = dict()
	acc = 0;
	for delay in delay_list[flow_id] :
		acc += delay_dict[flow_id][delay]
		cdf[flow_id][delay] = acc/total_length[flow_id] ;

# Statistics across all flows, for different delay i.e. QoS thresholds
flows= [flow_id for flow_id in cdf]
threshold_50_across_flows  = [ get_ccdf( 50, cdf[x] ) for x in flows ]
threshold_100_across_flows = [ get_ccdf(100, cdf[x] ) for x in flows ]
threshold_200_across_flows = [ get_ccdf(200, cdf[x] ) for x in flows ]

# sort to compute quantiles across flows.
threshold_50_across_flows.sort();
threshold_100_across_flows.sort();
threshold_200_across_flows.sort();

# print out 10th, 25th, 50th, 75th and 90th percentile
for quantile in [10, 25, 50, 75, 90] :
	print>>sys.stderr, " Threshold 50  ", quantile, "th percentile across flows ",threshold_50_across_flows[int((quantile/100.0)*len(threshold_50_across_flows))]

for quantile in [10, 25, 50, 75, 90] :
	print>>sys.stderr, " Threshold 100 ", quantile, "th percentile across flows ",threshold_100_across_flows[int((quantile/100.0)*len(threshold_100_across_flows))]

for quantile in [10, 25, 50, 75, 90] :
	print>>sys.stderr, " Threshold 200 ", quantile, "th percentile across flows ",threshold_200_across_flows[int((quantile/100.0)*len(threshold_200_across_flows))]
