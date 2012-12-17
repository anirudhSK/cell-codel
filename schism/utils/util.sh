#! /bin/bash
for i in `echo "49 50 51 52 53 54 55 56 57 58 59 60 70 80 90 98"`; do
flow0=`grep "from flow 0" $i-seed*.txt | wc -l`
flow1=`grep "from flow 1" $i-seed*.txt | wc -l`
total=`grep "from flow" $i-seed*.txt | wc -l`
echo "flow 0 : $i"
echo "scale=5;$flow0 / 9999990 " | bc
echo "flow 1 : $i"
echo "scale=5;$flow1 / 9999990 " | bc
echo "total : $i "
echo "scale=5;$total / 9999990 " | bc
echo "next iteration"
done
