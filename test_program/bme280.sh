#!/bin/sh

echo "Executing, bme280.out"
sudo stdbuf -o0 -e0 ./bme280.out | tee /home/pi/Documents/bme280`date +%Y%m%d_%H-%M-%S`.txt
#permissionが与えられてるのかわからず
