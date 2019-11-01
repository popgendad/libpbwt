#!/bin/bash
sampid=$1
for i in $(seq 0 1374)
do
	k=$(printf "%04d" $i)
	/tmp/libpbwt/test/ptest2 /data/EV92_${k}.pbwt 0.05 ${sampid} 0 399 > /data/sample_${sampid}_${k}.txt
done
