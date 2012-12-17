#! /bin/bash
for i in `echo "49 50 51 52 53 54 55 56 57 58 59 60 70 80 90 98"`; do
	echo "flow 0 : $i"
	time cat $i-seed* | python ../../utils/quantiles.py 0 > /dev/null
	echo "flow 1"
	time cat $i-seed* | python ../../utils/quantiles.py 1 > /dev/null
	echo "next iteration"
done
