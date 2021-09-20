#!/bin/bash

# bash runner for the test, compile and print data


numRuns=30 # run each test 30 times
tests=`echo sieve morse tetris snake`
fileName='testOutput.txt'

echo "Beginning tests" > $fileName

for test in $tests; do
	avg=0
	list=""
	for run in $(seq 1 $numRuns); do
		echo $run
		cd `echo $test/typed`
		res=`node main.js`
		avg=`echo $avg '+' $res | bc -l`
		list="$list $res"
		cd ../..
	done
	avg=`echo "$avg / $numRuns" | bc -l`
	echo $avg

	stdev=0
	for val in $list; do
		stdev=`echo "$stdev + ($val - $avg)^2" | bc -l`
	done 
	stdev=`echo "sqrt($stdev / ($numRuns - 1))" | bc -l`

	echo $test ' --> avg: ' $avg ' : stdev: ' $stdev >> $fileName
done
