CC ?= cc
TREE_SITTER ?= tree-sitter
CFLAGS ?= -O2

PARSER = parser/rcsh.so
SOURCES = src/parser.c src/scanner.c

.PHONY: all generate clean

all: $(PARSER)

$(PARSER): $(SOURCES)
	mkdir -p parser
	$(CC) -fPIC -I src -shared $(CFLAGS) $(SOURCES) -o $(PARSER)

generate:
	$(TREE_SITTER) generate

clean:
	rm -f $(PARSER)
