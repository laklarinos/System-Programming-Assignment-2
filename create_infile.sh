#!/bin/bash

#so this script is supposed to generate a sub directory for each
#country it encounters and create a specific number of .txt files
#for each of them. It should then insert a record refering to a 
#specific country in to the respective countries subdirectory
#using round robin.

#initial approach...
#take the countries file and create for each of these countries
#the subdirectory. For each subdirectory, create the number of 
#.txt files that the user gave us...

#create the directory
mkdir $2

declare -A countries
while IFS= read -r LINE || [[ -n "$LINE" ]];
do
    #2     
    i=0
    for word in $LINE
    do
        #3
        i=$(($i+1))
        if [[ $i = 5 ]]; then   
            if [[ -d "$2"/"$word" ]]
            then
                k=$((${countries[$word]}+1))
                if [[ $k > $3 ]]
                then
                    countries[$word]=1
                    printf "\n" >> $2/"$word"/"$word"-1.txt
                    echo -n $LINE >> $2/"$word"/"$word"-1.txt
                else
                    countries[$word]=$k
                    if [ -s $2/"$word"/"$word"-$k.txt ]
                    then
                        printf "\n" >> $2/"$word"/"$word"-$k.txt
                    fi  
                    echo -n $LINE >> $2/"$word"/"$word"-"$k".txt                  
                fi
                echo "$word exists on your filesystem."
            else
                countries[$word]=1
                mkdir $2/"$word"
                for ((counter=1; counter<=$3; counter++)); do
                    touch $2/"$word"/"$word"-"$counter".txt
                done
                echo -n $LINE >> $2/"$word"/"$word"-1.txt
            fi   
        fi
    done
done < inputFile.txt
