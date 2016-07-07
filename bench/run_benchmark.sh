#!/bin/bash

function clearFiles()
{
    rm *.dot
    rm *.pdf
    rm *.txt
}

function generateBenchList()
{
    find . -name "*.stp" | sort
}

function runBenchmarks()
{
    steinerbin="$1"
    
    declare -A metaHeuristicTypes
    metaHeuristicTypes=( ["c"]="--only-constructive" ["l"]="--localsearch" ["g"]="--grasp" ["t"]="--tabu" )
    
    declare -A heuristicTypes
    heuristicTypes=( ["t"]="thomson" ["tr"]="thomson-random" ["mtr"]="modthomson-random" )
    
    echo "filename,metaheuristic,heuristic,distance,time";
    >&2 echo "filename,metaheuristic,heuristic,distance,time";
    
    for b in $(generateBenchList)
    do
        for mh in "${!metaHeuristicTypes[@]}";
        do
            for h in "${!heuristicTypes[@]}";
            do
                filename=`echo "$b" | sed -r -e "s/\.\/(.*)\.stp/\1/g"`
                
                command="${metaHeuristicTypes["$mh"]} ${heuristicTypes["$h"]}";
                               
                results=`{ /usr/bin/time -f "%U" $steinerbin $command $b; } 2>&1 | paste -d, - -`;
                echo "$filename,$mh,$h,$results";
                >&2 echo "$filename,$mh,$h,$results";
            done           
        done
    done
    
    find . -name "*.dot" | sort | xargs -n 1 -I {} neato -n -Goverlap=scale -Tpdf {} -o {}.pdf
}

function runSteiner()
{
    clearFiles
    results=$(runBenchmarks $1)
}

runSteiner $1 > results.txt
