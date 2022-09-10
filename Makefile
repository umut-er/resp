CC=g++
CFLAGS=-Wall -O2

resp: resp.cpp
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -f resp
