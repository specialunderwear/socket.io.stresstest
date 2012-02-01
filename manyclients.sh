#! /bin/bash
echo "Spawning $1 processes"
T="$(date +%s)"
FAIL=0
pidlist=""
for (( i = 0; i <= $1 ; i++ ))
do
    if [ $4 ]; then
        ./websocketclient $2 $3 &
	pidlist="$pidlist $!"
	sleep 0.001s
    else
        ./websocketclient $2 $3 > /dev/null &
	pidlist="$pidlist $!"
	sleep 0.001s
    fi
done
echo "Done spawning processes in $(($(date +%s)-T)) seconds"
echo "Waiting for completion now ..."
WAIT=0
for job in $pidlist
do
    let WAIT+=1
    wait $job || let "FAIL+=1"
done

if [ "$FAIL" == "0" ];
then
echo "All clients completed succesfully."
else
echo "O noes ($FAIL) clients crashed!"
fi

T="$(($(date +%s)-T))"
echo "Time in seconds: ${T}"
