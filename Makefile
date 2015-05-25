all: rbtree.c
	gcc -O3 -Wall -Wextra -std=gnu89 $^ -o bin
