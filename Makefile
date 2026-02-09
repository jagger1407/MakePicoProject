CC = gcc
SRC = makepicoproject.c
BIN = build

all: debug release

debug: CFLAGS = -g -O0
debug: $(BIN)/debug/MakePicoProject

release: CFLAGS = -O2
release: $(BIN)/release/MakePicoProject


$(BIN)/debug/MakePicoProject: $(SRC) | $(BIN)/debug
	$(CC) $(SRC) $(CFLAGS) -o $@

$(BIN)/release/MakePicoProject: $(SRC) | $(BIN)/release
	$(CC) $(SRC) $(CFLAGS) -o $@

$(BIN) $(BIN)/debug $(BIN)/release:
	mkdir -p $@

clean:
	rm -rf $(BIN)
