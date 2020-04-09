# Default to debugging flags. Set to -O3 if you want optimisations
# (either change it here, or type 'make CFLAGS=-O3' in the shell).
CFLAGS?=-g -Wall -Wno-unused-value

all: hare_tortoise

hare_tortoise: hare_tortoise.c hare_tortoise_runner.c cs_thread.h
	$(CC) $(CFLAGS) -o hare_tortoise hare_tortoise_runner.c -lpthread 

clean:
	$(RM) hare_tortoise
	$(RM) -r *.dSYM

run: hare_tortoise
	./hare_tortoise test-file=Testcases/test1.txt
	./hare_tortoise test-file=Testcases/test2.txt
	./hare_tortoise test-file=Testcases/test3.txt
