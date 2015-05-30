Tiny Red-Black Tree [![Build status][travis-i]][travis-a]
========
```sh
make
./bin info
./bin < test/fixtures/test03
```

#### Test
Performs both functional tests and memory checks.
```sh
make debug
test/do
```

#### Debugging
```sh
make debug
gdb ./bin
valgrind --leak-check=yes ./bin < test/fixtures/delete02 1>/dev/null
```

[travis-i]: https://travis-ci.org/simnalamburt/tiny-rbtree.svg?branch=master
[travis-a]: https://travis-ci.org/simnalamburt/tiny-rbtree
