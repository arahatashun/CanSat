#!/bin/sh

echo "Executing, gps.out"
sudo stdbuf -o0 -e0 ./gps.out | tee /home/pi/Documents/gps`date +%Y%m%d_%H-%M-%S`.txt
#permissionが与えられてるのかわからず
