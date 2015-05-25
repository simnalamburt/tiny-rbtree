all: rbtree.c
	gcc -O3 -Wall -Wextra -std=gnu89 $^ -o bin

debug: rbtree.c
	gcc -g -Og -Wall -Wextra -std=gnu89 $^ -o bin
