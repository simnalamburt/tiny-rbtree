CFLAGS = -Wall -Wextra -Wpedantic -Werror -std=c99

all: rbtree.c
	$(CC) -DNDEBUG -O3 $(CFLAGS) $^ -o bin

debug: rbtree.c
	$(CC) -g $(CFLAGS) $^ -o bin
