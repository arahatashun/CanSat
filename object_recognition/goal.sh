#!/bin/bash
trap 'echo "SIGIN Handle";exit 1' 2
timeout -sSIGINT 100 sudo ./goal.out
echo "TIME IS OUT"
