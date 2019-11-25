#!/bin/bash
sampid=$1
/tmp/libpbwt/test/ptest2 /data/HumanOriginsExample.pbwt 0.5 ${sampid} 0 729 > /data/sample_${sampid}.txt
