
all: restart

restart: restart.c restart.h
	gcc -Wall -Wno-unused-result -O -g -o restart restart.c
	-@ echo "Done"

clean:
	-rm -f *~
