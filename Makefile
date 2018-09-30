all: redeye

SRC = lidar.c alerter.c recorder.c avconv_util.c server_watchdog.c redeye_device.c
CC = gcc
CFLAGS = -Iinclude -pthread -lcurl -lwiringPi -ltof

redeye: $(SRC)
	$(CC) $(SRC) -o redeye $(CFLAGS)

clean:
	rm -f redeye
