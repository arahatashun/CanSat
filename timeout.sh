#!/bin/bash
trap 'echo SIGIN Handle; sudo ./sigintHandle.out; exit 1' 2
timeout -sSIGINT 1080 sudo sh end2end.sh
echo "TIME IS OUT"
