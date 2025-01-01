CC = clang
CFLAGS = -Wall -Wextra -Werror $(shell pkg-config --cflags gmp)
LFLAGS = $(shell pkg-config --libs gmp)

all: keygen encrypt decrypt

decrypt: ss.o decrypt.o numtheory.o randstate.o
	$(CC) -o decrypt ss.o decrypt.o numtheory.o randstate.o $(LFLAGS)

encrypt: ss.o encrypt.o numtheory.o randstate.o
	$(CC) -o encrypt ss.o encrypt.o numtheory.o randstate.o $(LFLAGS)

keygen: ss.o keygen.o numtheory.o randstate.o
	$(CC) -o keygen ss.o keygen.o numtheory.o randstate.o $(LFLAGS)

numtheory: numtheory.o randstate.o
	$(CC) -o $@ $^ $(LFLAGS)

test : numtheory.c randstate.c
	$(CC) -o $@ $^ $(LFLAGS) $(CFLAGS)

decrypt.o : decrypt.c
	$(CC) $(CFLAGS) -c $< 
encrypt.o : encrypt.c
	$(CC) $(CFLAGS) -c $< 
keygen.o : keygen.c 
	$(CC) $(CFLAGS) -c $<
ss.o: ss.c
	$(CC) $(CFLAGS) -c $<

numtheory.o: numtheory.c
	$(CC) $(CFLAGS) -c $<
randstate.o: randstate.c
	$(CC) $(CFLAGS) -c $<

format:
	clang-format -i -style=file *.[ch]

clean:
	rm -f decrypt encrypt keygen numtheory randstate *.o
