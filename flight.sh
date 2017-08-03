#!/bin/bash
#NOTE 事前に実行ファイルの用意
trap 'echo "SIGIN Handle"; sudo ./sigintHandle.out; exit 1' 2
echo "flight sequence start
sudo stdbuf -o0 -e0 ./flight.out | tee /home/pi/Documents/`date +%Y%m%d_%H-%M-%S`.txt
