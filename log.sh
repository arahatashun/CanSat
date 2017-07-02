#!/bin/sh

echo "Enter program name (excluding .out)"
read program
echo "Executing, $program.out"
sudo stdbuf -o0 -e0 ./$program.out | tee ~/Documents/`date +%Y%m%d_%H-%M-%S`.txt
#permissionが与えられてるのかわからず
