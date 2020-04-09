# Default to debugging flags. Set to -O3 if you want optimisations
# (either change it here, or type 'make CFLAGS=-O3' in the shell).
CFLAGS?=-g -Wall -Wno-unused-value

all: reaction

reaction: reaction_runner.c reaction.c cs_thread.h
	$(CC) $(CFLAGS) -o reaction reaction_runner.c -lpthread 

clean:
	$(RM) reaction
	$(RM) -r *.dSYM

run: reaction
	./reaction 0
	./reaction 0
	./reaction 20
	./reaction 20
	./reaction 40
	./reaction 40
	./reaction 60
	./reaction 60
	./reaction 80
	./reaction 80
	./reaction 100
	./reaction 100
