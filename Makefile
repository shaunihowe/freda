ARCH  = native
VERSION = 0.999
CC    = cc
COPTS = -g -Wall -O3 -march=$(ARCH) -flto -s
LIBS  = -lpthread -lm

all: compile
	$(CC) $(COPTS) obj/freda.o obj/api.o obj/bitboard.o obj/board.o obj/eval_weights.o obj/eval.o obj/hash.o obj/search.o $(LIBS) -o bin/freda-$(VERSION)-$(ARCH)

compile: clean
	$(CC) $(COPTS) $(LIBS) -o obj/api.o -c src/api.c
	$(CC) $(COPTS) $(LIBS) -o obj/bitboard.o -c src/bitboard.c
	$(CC) $(COPTS) $(LIBS) -o obj/board.o -c src/board.c
	$(CC) $(COPTS) $(LIBS) -o obj/eval_weights.o -c src/eval_weights.c
	$(CC) $(COPTS) $(LIBS) -o obj/eval.o -c src/eval.c
	$(CC) $(COPTS) $(LIBS) -o obj/hash.o -c src/hash.c
	$(CC) $(COPTS) $(LIBS) -o obj/search.o -c src/search.c
	$(CC) $(COPTS) $(LIBS) -o obj/freda.o -c src/freda.c

clean:
	rm -f obj/*.o

run: all
	bin/freda-$(VERSION)-$(ARCH)
