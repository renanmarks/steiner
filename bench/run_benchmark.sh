#!/bin/bash


benchmarkfile=benchmarks.txt
distancefile=distance.txt
timefile=time.txt

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

function generateHeuristicResultsList()
{
    find . -name "*$1-$2.txt" | sort
}

function runSteiner()
{
    steinerbin="$1"
    
    declare -A metaHeuristicTypes
    metaHeuristicTypes=( ["c"]="--only-constructive" ["l"]="--localsearch" ["g"]="--grasp" ["t"]="--tabu" )
    
    declare -A heuristicTypes
    heuristicTypes=( ["t"]="thomson" ["tr"]="thomson-random" ["mtr"]="modthomson-random" )
    
    for b in $(generateBenchList)
    do
        for mh in "${!metaHeuristicTypes[@]}";
        do
            for h in "${!heuristicTypes[@]}";
            do
                distancefile="distance_$mh-$h.txt"
                timefile="time_$mh-$h.txt"
                command="${metaHeuristicTypes["$mh"]} ${heuristicTypes["$h"]}";
                filename=`echo "$b" | sed -r -e "s/\.\/(.*)\.stp/\1/g"`
                
                touch $distancefile
                touch $timefile
                
                results=$( { /usr/bin/time -f "%U" $steinerbin $command $b; } 2>&1 );
                results=$( echo $results | sed -e "s/\n//g" );
                echo "$filename $mh $h $results"
                
                #/usr/bin/time -f "%U" -o $timefile $steinerbin $command $b \> $distancefile
                #cat benchmarkfile | { xargs -n 1 time -f "%U" $steinerbin $command > distancefile ; } 2> timefile
               
                #resultsfile=`echo "$b" | sed -r -e "s/\.\/(.*)\.txt/results-\1-$mh-$h.txt/g"`
                #commandline=`generateHeuristicResultsList $mh $h | paste - -`;
                #commandline="paste -d, $commandline > $resultsfile";
                
                #echo $commandline;
            done
            
            echo "";
            #echo "paste" benchmarkfile distancefile timefile -d, > results
        done
    done
    
    #paste benchmarkfile distancefile timefile -d, > results
    #find . -name "*.dot" | sort | xargs -n 1 -I {} neato -n -Goverlap=scale -Tpdf {} -o {}.pdf
}

clearFiles
runSteiner $1

