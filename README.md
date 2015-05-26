Tiny Red-Black Tree [![Build status][travis-i]][travis-a]
========
```sh
make
./bin info
./bin < test/fixtures/test03
```

#### Test
```sh
make debug
test/do
```

#### Debugging
```sh
make debug
gdb ./bin
```

#### Memory Check
```sh
make debug
valgrind --tool=memcheck --leak-check=yes ./bin < test/fixtures/test03 1>/dev/null
```

[GeeksQuiz]: http://geeksquiz.com/c-program-red-black-tree-insertion
[travis-i]: https://travis-ci.org/simnalamburt/tiny-rbtree.svg?branch=master
[travis-a]: https://travis-ci.org/simnalamburt/tiny-rbtree
