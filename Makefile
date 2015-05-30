CFLAGS = -Wall -Wextra -std=gnu89 -Werror=implicit-function-declaration

all: rbtree.c
	$(CC) -DNDEBUG -O3 $(CFLAGS) $^ -o bin

debug: rbtree.c
	$(CC) -g $(CFLAGS) $^ -o bin
