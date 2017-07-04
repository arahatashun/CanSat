#!/bin/bash
#NOTE 事前に実行ファイルの用意
trap 'echo SIGIN Handle; sudo ./sigintHandle.out; exit 1' 2
echo "start gut cut"
sudo ./test_program/gut.out
echo "start ground run"
echo "Executing, ground_compass.out"
sudo stdbuf -o0 -e0 ./ground_compass.out | tee /home/pi/Documents/`date +%Y%m%d_%H-%M-%S`.txt
echo "Executing, goal.out"
sudo stdbuf -o0 -e0 ./object_recognition/goal.out | tee /home/pi/Documents/`date +%Y%m%d_%H-%M-%S`.txt
