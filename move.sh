#!/bin/bash
# either echo "0=50%" >/dev/servoblaster or echo 0=150 >/dev/servoblaster will set middle position
# from 0% to 100% or from 50 to 250  (~ 500 us to 2500 us in 10 us steps)  

trap 'sigint'  INT #(SIGINT = 2 check kill -l)

#trap 'sigterm'  EXIT
# Exit value of non zero is not ideal
# exit 0 = success
# here - exit value is being used to get current position
# exit values should be in the range of 0-255
sigint()
{
#echo "signal INT received, script ending"
#echo "r2 value is "
#echo $r2
#echo $r2 > "$file"
exit $r2
#exit 0
}




SERVO=$1 # Which servo is pan and which one is tilt (0 or 1)
STEPS=$2 # determines the speed of movement
SLEEP=$3
DIR=$4 # Increase or Decrease as INC or DEC
VAL=$5 #Max/min Pan/TIlt - With INC - give MAX Val, with DEC - give MIN Val
CURR_VAL=$6



r2=$(($CURR_VAL))

if [ "$DIR" = "INC" ]; then
if [ "$r2" -le $VAL ]; then # don't want to turn more than 95% ~ 2400 us ~ 240 steps of 10 us each
while [ "$r2" -le $VAL ];do
((r2=r2+STEPS))
echo $SERVO"="$r2 > /dev/servoblaster 
sleep $SLEEP # TODO by combining sleep and steps - can make more configs of speed
done
fi

fi

if [ "$DIR" = "DEC" ]; then
if [ "$r2" -ge $VAL ]; then # don't want to turn less than 5% ~ 550 us ~ 55 steps of 10 us each
while [ "$r2" -ge $VAL ];do
((r2=r2-STEPS))
echo $SERVO"="$r2 > /dev/servoblaster 
sleep $SLEEP # TODO by combining sleep and steps - can make more configs of speed
done
fi

fi
exit $r2

