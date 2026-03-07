.PHONY: bin lib clean test

CC = gcc
CFLAGS = -MMD -MP -Wall -Wextra -Wconversion -g -Iinclude -O0

LIBRARY = build/libdihah.a
BIN = build/dihahc

SRC := $(shell find src -name "*.c")
OBJ := $(patsubst src/%.c,build/%.o,$(SRC))

SRCBIN := $(shell find srcbin -name "*.c")

TEST := $(shell find test -name "*.c")
TESTOBJ := $(patsubst test/%.c,build/test/%.o,$(TEST))
TESTBIN := $(TESTOBJ:.o=.exe)

runtest: test
	$(foreach test, $(TESTBIN), ./$(test);)

run: $(BIN)
	./$(BIN)

$(BIN): lib
	$(CC) $(SRCBIN) -Lbuild -ldihah -o $(BIN) $(CFLAGS)

lib: $(LIBRARY)

$(LIBRARY): $(OBJ)
	ar rcs $@ $^

build/%.o: src/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

build/test/%.o: test/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

# link each test
$(TESTBIN): build/test/%.exe: build/test/%.o lib
	$(CC) $< -Lbuild -ldihah -o $@

test: $(TESTBIN)

clean:
	rm -rf build

-include $(OBJ:.o=.d)
-include $(TESTOBJ:.o=.d)
