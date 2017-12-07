
visualpi:build\net_daemon.o build\oled.o
	gcc -o $@ $^ -I/usr/local/include -L/usr/local/lib -lwiringPi

%.o:%.c
	gcc -c $^ -o $@ -I/usr/local/include -L/usr/local/lib -lwiringPi

.PHONY: clean

clean:
	rm -rf *.o visualpi
