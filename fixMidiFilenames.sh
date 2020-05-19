#!/bin/bash

declare -a Files=(bin/data/pd/data/midi/*)

#echo ${Files[1]]}

len=${#Files[@]}

echo nb files : $len

i=0

#while [$i<$len] ; do 
#    echo $i
#    i=$(($i + 1))
#done
 
for x in "${Files[@]}" ; do
    y=`echo $x | sed -e 's/ /_/g'`
    mv "$x" $y
done
    

