#!/bin/bash
#NOTE 事前に実行ファイルの用意
sudo modprobe bcm2835-v4l2 
echo "camera driver setup"
trap 'echo "SIGIN Handle"; sudo ./sigintHandle.out; exit 1' 2
echo "Executing, goal.out"
sudo stdbuf -o0 -e0 ./object_recognition/goal.out | tee /home/pi/Documents/`date +%Y%m%d_%H-%M-%S`.txt
