#!/bin/sh
trap 'echo SIGIN Handle;exit 1' 2
timeout -sSIGINT 100 sudo ./goal.output
echo "TIME IS OUT"
