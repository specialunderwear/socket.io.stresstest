#! /bin/bash
echo "Spawning $1 processes"
for (( i = 0; i <= $1 ; i++ ))
do
    ./websocketclient localhost:81 input.json &
done