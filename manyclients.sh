#! /bin/bash
echo "Spawning $1 processes"
FAIL=0
for (( i = 0; i <= $1 ; i++ ))
do
    ./websocketclient $2 $3 > /dev/null &
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