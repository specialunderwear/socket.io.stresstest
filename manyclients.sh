#! /bin/bash
echo "Spawning $1 processes"
T="$(date +%s)"
FAIL=0
for (( i = 0; i <= $1 ; i++ ))
do
    if [ $4 ]; then
        ./websocketclient $2 $3 &
    else
        ./websocketclient $2 $3 > /dev/null &
    fi
done
for job in `jobs -p`
do
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
