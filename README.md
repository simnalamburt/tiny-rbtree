Tiny Red-Black Tree
========
### Prerequisites
- C99 compliant compiler
- GNU Make
- Ruby 2.0+ (just for test)

### Instructions
```bash
make
./bin info
./bin < test/fixtures/test03

# Testing: Performs both functional tests and memory checks
make debug
test/do

# Debugging
make debug
gdb ./bin
valgrind --leak-check=yes ./bin < test/fixtures/delete02 1>/dev/null
```

--------

MIT License
