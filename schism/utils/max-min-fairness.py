#! /usr/bin/python
# Calculate weighted max min fair allocations

import copy
# INPUT : Desired Rates, Weights, capacity
desired={0:0.036000 , 1:0.072, 2:0.108, 3:0.144, 4:0.18, 5:0.216,6:0.252,7:0.288, 8:0.324,9:0.36}
for user in desired :
	desired[user] *=1000715
weights={0:1.0,1:2.0,2:3.0,3:4.0,4:5.0,5:6.0,6:7.0,7:8.0,8:9.0,9:10.0}
for weight in weights :
	weights[weight]**=(+0.5);
norm_factor=sum(weights.values())
for weight in weights :
	weights[weight]/=norm_factor
weight_copy=copy.deepcopy(weights)
total_capacity=1000715

# OUTPUT: Allocations.
final_share=dict()

# book-keeping
current_share=dict()
for user in desired :
	current_share[user]=0
capacity = total_capacity

# core algm
while  ( len(weights) > 0 ) and ( capacity > 0 ) :
	total_shares = sum(weights.values())
	unit_share   = capacity/total_shares;
	user_list = weights.keys()
	for user in user_list :
		fair_share = unit_share * weights[ user ];
		current_share[ user ] += fair_share
		if current_share[ user ] >= desired[ user ] :
			spare_capacity = (current_share[ user ] - desired[ user ]);
			final_share[ user ] = desired[ user ]
			del current_share[ user ]
			del weights[ user ]
			capacity = capacity + spare_capacity - fair_share;
		else :
			capacity = capacity - fair_share ;

# finalize allocations
for share in current_share :
	final_share[share]=current_share[share]

# print
for share in final_share :
	bottleneck=True
	if final_share[share] == desired[share] :
		bottleneck=False
	print "user ", share,"weight %.5f"%weight_copy[share],"desired %7.0f"%desired[share],"allocated %7.0f"%final_share[share]," bottleneck ",bottleneck

# aggregate stats
print "=============TOTAL STATS===================="
print "capacity", total_capacity
print "demand", sum(desired.values())
print "amount allocated", sum(final_share.values())
