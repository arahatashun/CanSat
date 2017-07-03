#!/bin/sh
trap 'echo -e "SIGIN Handle\n";sudo ./sigintHandle.out' 2
timeout -sSIGINT 1080 sudo sh ground.sh
echo "TIME IS OUT"
