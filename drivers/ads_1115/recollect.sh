picocom -b 115200 /dev/ttyUSB0 &
sleep 3
echo "start"
touch radio_sun.log
echo "" > radio_sun.log
echo "cat to radio"
#cat /dev/ttyUSB0 | while read i; do echo "${i}" >> radio_sun.log  ; done
cat /dev/ttyUSB0 | tee radio_sun.log 
echo "plot"
#cat radio_sun.log | ./plot_evolution.pl
#sudo kill -9 $(pidof picocom) 
