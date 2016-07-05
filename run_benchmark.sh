#!/bin/bash

steinerbin="$1"
outputfile="distancetime.txt"

rm *.dot
rm *.pdf
rm *.txt
find . -name "*.stp" | sort > benchmarks.txt
cat benchmarks.txt | { xargs -n 1 time -f "%U" $steinerbin --only-constructive thomson > distance.txt ; } 2> time.txt
paste benchmarks.txt distance.txt time.txt -d,
find . -name "*.dot" | sort | xargs -n 1 -I {} neato -n -Goverlap=scale -Tpdf {} -o {}.pdf
