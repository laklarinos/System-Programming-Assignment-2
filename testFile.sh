#!/bin/bash
#entries=($(shuf -i 1-15 -n $3))  
flag=0
if [[ $4 = "NO" ]]; then
    entries=($(shuf -i 1-9999 -n $3))  
else [[ $4 = "YES" ]];
    flag=1
    for ((i=0; i<$3; i++)); do
        entries[i]=$((1 + $RANDOM % 9999));
    done
fi
n=0

#read countries line by line
countryNum=0
while IFS= read -r line || [[ -n "$line" ]];
do
  countryArray[$countryNum]="$line"
  countryNum=$(($countryNum+1))
done < "$1"

#read viruses line by line
virusNum=0

while IFS= read -r line || [[ -n "$line" ]];
do
  virusArray[$virusNum]="$line"
  virusNum=$(($virusNum+1))
done < "$2"

printf "$3"

for((i=0; i<$3; i++)); do    
        
    if [[ "$i" -gt 0 ]]; then
        printf "\n" >>inputFile.txt
    fi
    # Check if user wants duplication or not...
    if [[ "$flag" -eq 1 ]]; 
    then
        if [[ $(($i%22)) = 0 ]]; 
        then
            printf ''"${entries[i-10]}"' '>>inputFile.txt
        else 
            printf ''"${entries[i]}"' '>>inputFile.txt
        fi
    else 
        printf ''"${entries[i]}"' '>>inputFile.txt
    fi    
    

    numOfChars=$((3 + $RANDOM % 13))
    tr -dc A-Za-z</dev/urandom | head -c "$numOfChars" >> inputFile.txt; printf ' '>>inputFile.txt
    
    #generate a number in range 2-13 for surnames...
    numOfChars=$((2 + $RANDOM % 13))
    tr -dc A-Za-z</dev/urandom | head -c "$numOfChars" >> inputFile.txt; printf ' '>>inputFile.txt
    
    age=$((1 + $RANDOM % 120))
    printf "$age " >>inputFile.txt    

    #generate a number between 0 and $index -> indicates the number of countries...
    #country=$((0 + $RANDOM % "$countryNum")) 

    country=$(shuf -i 0-$(($countryNum-1)) -n 1)

    printf "${countryArray[$country]} ">>inputFile.txt

    #virus=$((0 + $RANDOM % "$virusNum")) 
    virus=$(shuf -i 0-$(($virusNum-1)) -n 1)

    printf "${virusArray[$virus]}">>inputFile.txt

    if [[ $(($RANDOM % 2)) = 0 ]]; then
        printf " NO" >>inputFile.txt
    else 
        printf " YES" >>inputFile.txt
        day=$(shuf -i 1-30 -n 1)
        month=$(shuf -i 1-12 -n 1)
        year=$(shuf -i 1901-2021 -n 1)
        printf " $day-$month-$year" >> inputFile.txt        
    fi
done