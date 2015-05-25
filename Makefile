all: rbtree.c
	gcc -O3 -Wall -Wextra -std=gnu89 $^ -o bin

test: all
	@test/test

debug: rbtree.c
	gcc -g -Wall -Wextra -std=gnu89 $^ -o bin
