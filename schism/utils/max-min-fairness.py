#! /usr/bin/python
# Calculate weighted max min fair allocations

# INPUT : Desired Rates, Weights, capacity
desired={0:0.036000 , 1:0.072, 2:0.108, 3:0.144, 4:0.18, 5:0.216,6:0.252,7:0.288, 8:0.324,9:0.36}
for user in desired :
	desired[user] *=1000715
weights={0:1,1:1,2:1,3:1,4:1,5:1,6:1,7:1,8:1,9:1}
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

#print
print "desired", desired
print "allocated", final_share
print "capacity", total_capacity
print "demand", sum(desired.values())
print "amount allocated", sum(final_share.values())
