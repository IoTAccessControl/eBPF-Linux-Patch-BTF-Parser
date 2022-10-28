CFLAGS = -g -Wall
LDFLAGS = -L../openssl-3.0.5/ -lcrypto

all: hash_file

hash_file:
	$(CC) $(CFLAGS) hash_file.c -o $@ $< $(LDFLAGS)

clean:
	$(RM) hash_file
