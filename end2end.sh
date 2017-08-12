#!/bin/bash
#NOTE 事前に実行ファイルの用意
sudo modprobe bcm2835-v4l2 #カメラのドライバー
trap 'echo "SIGIN Handle"; sudo ./sigintHandle.out; exit 1' 2
echo "start flight sequence"
sudo stdbuf -o0 -e0 ./flight.out | tee /home/pi/Documents/`date +%Y%m%d_%H-%M-%S`.txt
echo "start forward"
sudo ./forward.out
echo "start ground running sequence"
echo "Executing, ground_compass.out"
sudo stdbuf -o0 -e0 ./ground_compass.out | tee /home/pi/Documents/`date +%Y%m%d_%H-%M-%S`.txt
echo "Executing, goal.out"
sudo stdbuf -o0 -e0 ./object_recognition/goal.out | tee /home/pi/Documents/`date +%Y%m%d_%H-%M-%S`.txt
