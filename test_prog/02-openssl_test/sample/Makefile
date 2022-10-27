CFLAGS = -g -Wall
LDFLAGS = -L../openssl-1.1.1q/ -lcrypto

all: hash_file

hash_file:
	$(CC) $(CFLAGS) hash_file.c -o $@ $< $(LDFLAGS)

clean:
	$(RM) hash_file
