VERSION = 1.1
OPTS_NAME = native
OPTS  = -march=native
CC    = cc
COPTS = -Wall -O3 -ftree-vectorize -fno-strict-aliasing $(OPTS)
LIBS  = -lpthread -lm

compile: clean
	$(CC) $(COPTS) $(LIBS) -o obj/bitboard.o -c src/bitboard.c
	$(CC) $(COPTS) $(LIBS) -o obj/board.o -c src/board.c
	$(CC) $(COPTS) $(LIBS) -o obj/eval_weights.o -c src/eval_weights.c
	$(CC) $(COPTS) $(LIBS) -o obj/eval.o -c src/eval.c
	$(CC) $(COPTS) $(LIBS) -o obj/hash.o -c src/hash.c
	$(CC) $(COPTS) $(LIBS) -o obj/xboard.o -c src/xboard.c
	$(CC) $(COPTS) $(LIBS) -o obj/uci.o -c src/uci.c
	$(CC) $(COPTS) $(LIBS) -o obj/freda.o -c src/freda.c
	$(CC) $(COPTS) $(LIBS) -o obj/search.o -c src/search.c

clean:
	rm -f obj/*.o

all: compile
	$(CC) $(COPTS) obj/xboard.o obj/freda.o obj/bitboard.o obj/board.o obj/eval_weights.o obj/eval.o obj/hash.o obj/search.o $(LIBS) -o bin/freda_xboard-$(VERSION)-$(OPTS_NAME)
	$(CC) $(COPTS) obj/uci.o obj/freda.o obj/bitboard.o obj/board.o obj/eval_weights.o obj/eval.o obj/hash.o obj/search.o $(LIBS) -o bin/freda_uci-$(VERSION)-$(OPTS_NAME)

