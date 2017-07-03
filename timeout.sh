#!/bin/sh
trap 'echo -e "SIGIN Handle\n";sudo ./sigintHandle.out;exit' 2
timeout -sSIGINT 1080 sudo sh end2end.sh
echo "TIME IS OUT"
