#!/bin/bash



for cnline in `cat _cnstl.txt`
do
   echo $cnline
	
   LINEOBJ=`echo $cnline |sed -r 's/^([a-zA-Z]+)\s+([0-9]+)\s+(.*)$/\1/g'`
   LINECOORD=`echo $cnline |sed -r 's/^([a-zA-Z]+)\s+([0-9]+)\s+(.*)$/\3/g'`
   echo "OBJ="$LINEOBJ" coord="$LINECOORD
   
done
   