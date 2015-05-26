CFLAGS = -Wall -Wextra -std=gnu89 -Werror=implicit-function-declaration

all: rbtree.c
	gcc -DNDEBUG -O3 $(CFLAGS) $^ -o bin

debug: rbtree.c
	gcc -g $(CFLAGS) $^ -o bin
