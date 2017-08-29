stty -f /dev/tty.usbserial-AL01T0PA pass8 raw -cstopb
cat /dev/tty.usbserial-AL01T0PA | tee ~/Documents/log.txt
