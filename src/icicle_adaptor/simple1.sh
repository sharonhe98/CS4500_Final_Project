# !/bin/bash

# Storing substring reference: https://stackoverflow.com/questions/4457953/how-to-store-a-substring-of-the-output-of-time-function-in-bash-script
# Found on Thursday Feb 6, 2020

array=()

for i in {1..100}
	do
		echo -n "Result for run $i : "
		TIMEFORMAT=%R
		exec 3>&1 4>&2
		echo $( { time $1 -f $3 -from 0 -len 100000 -is_missing_idx $10 1>&3 2>&4; } 2>&1 ) >> icicle_times_missing.txt
		exec 3>&- 4>&-
		#echo $TIMEDRES
		  
done

echo ${array[@]}