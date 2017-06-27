#!/bin/sh

read program
echo "Executing, $program"
sudo stdbuf -o0 -e0 ./$program | tee log/`date +%Y%m%d_%H-%M-%S`.txt
