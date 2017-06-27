#!/bin/sh

read program
echo "Executing, $program.out"
sudo stdbuf -o0 -e0 ./$program.out | tee log/`date +%Y%m%d_%H-%M-%S`.txt
#permissionが与えられてるのかわからず
