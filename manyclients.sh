#! /bin/bash

FAIL=0
VERBOSE=0
SLEEP="0.01"

# parse options
while getopts "vw:d" opt; do
  case $opt in
    v)
      VERBOSE=1
      ;;
    w)
      SLEEP=$OPTARG
      ;;
    \?)
      echo "Invalid option: -$OPTARG" >&2
      exit 1
      ;;
    :)
      echo "Option -$OPTARG requires an argument." >&2
      exit 1
      ;;
  esac
done

# remove option from args
shift $(($OPTIND - 1))

# warn if not enough args
if [ $# != 3 ]; then
    echo "Usage manyclients.sh [-v] [-w seconds] 100 http(s)://host/ inputfile"
    exit
fi

echo "Spawning $1 processes, waiting for $SLEEP seconds between spawns."

T="$(date +%s)"
for (( i = 0; i < $1 ; i++ ))
do
    if [ $VERBOSE != "0" ]; then
        ./websocketclient $2 $3 &
	PIDLIST="$PIDLIST $!"
	sleep $SLEEP
    else
        ./websocketclient $2 $3 > /dev/null &
	PIDLIST="$PIDLIST $!"
	sleep $SLEEP
    fi
done
echo -en "\033[1;31mDone spawning processes in $(($(date +%s)-T)) seconds\033[0m \n"
echo "Waiting for completion now ..."
WAIT=0
for job in $PIDLIST
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
