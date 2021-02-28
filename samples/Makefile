
.PHONY: all
all: driver prog wrong

.PHONY: driveer
driver: driver.c
	gcc driver.c -o driver

.PHONY: prog
prog: prog.c
	gcc prog.c -o prog

.PHONY: wrong
wrong: wrong.c
	gcc wrong.c -o wrong

.PHONY: loop
loop: loop.c
	gcc loop.c -o loop

clean:
	rm -f driver prog wrong
