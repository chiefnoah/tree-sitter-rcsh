#include "tree_sitter/parser.h"

#include <ctype.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

enum TokenType {
	HEREDOC_MARKER,
	HEREDOC_BODY,
};

typedef struct {
	char delimiter[128];
	uint16_t delimiter_len;
} Scanner;

void *tree_sitter_rcsh_external_scanner_create(void) {
	Scanner *scanner = calloc(1, sizeof(Scanner));
	return scanner;
}

void tree_sitter_rcsh_external_scanner_destroy(void *payload) {
	free(payload);
}

unsigned tree_sitter_rcsh_external_scanner_serialize(void *payload, char *buffer) {
	Scanner *scanner = payload;
	if (scanner->delimiter_len == 0)
		return 0;
	buffer[0] = (char)scanner->delimiter_len;
	memcpy(buffer + 1, scanner->delimiter, scanner->delimiter_len);
	return scanner->delimiter_len + 1;
}

void tree_sitter_rcsh_external_scanner_deserialize(void *payload, const char *buffer, unsigned length) {
	Scanner *scanner = payload;
	memset(scanner, 0, sizeof(Scanner));
	if (length == 0)
		return;
	scanner->delimiter_len = (uint8_t)buffer[0];
	if (scanner->delimiter_len >= sizeof(scanner->delimiter))
		scanner->delimiter_len = sizeof(scanner->delimiter) - 1;
	if (scanner->delimiter_len + 1 > length)
		scanner->delimiter_len = length - 1;
	memcpy(scanner->delimiter, buffer + 1, scanner->delimiter_len);
	scanner->delimiter[scanner->delimiter_len] = '\0';
}

static bool delimiter_line(TSLexer *lexer, Scanner *scanner) {
	uint16_t i;

	for (i = 0; i < scanner->delimiter_len; i++) {
		if (lexer->lookahead != scanner->delimiter[i])
			return false;
		lexer->advance(lexer, false);
	}

	return lexer->lookahead == '\n' || lexer->lookahead == '\r' || lexer->lookahead == '\0';
}

static bool scan_marker(TSLexer *lexer, Scanner *scanner) {
	uint16_t len = 0;

	while (lexer->lookahead == ' ' || lexer->lookahead == '\t')
		lexer->advance(lexer, true);

	while (lexer->lookahead != '\0' &&
	       lexer->lookahead != '\n' &&
	       lexer->lookahead != '\r' &&
	       lexer->lookahead != '\'' &&
	       !isspace((unsigned char)lexer->lookahead)) {
		if (len + 1 < sizeof(scanner->delimiter)) {
			scanner->delimiter[len++] = (char)lexer->lookahead;
		}
		lexer->advance(lexer, false);
	}

	if (len == 0)
		return false;

	scanner->delimiter[len] = '\0';
	scanner->delimiter_len = len;
	lexer->result_symbol = HEREDOC_MARKER;
	return true;
}

static bool scan_body(TSLexer *lexer, Scanner *scanner) {
	bool at_line_start = true;

	if (scanner->delimiter_len == 0)
		return false;

	if (lexer->lookahead == '\'')
		return false;

	lexer->mark_end(lexer);
	while (lexer->lookahead != '\0') {
		if (at_line_start && delimiter_line(lexer, scanner)) {
			scanner->delimiter_len = 0;
			scanner->delimiter[0] = '\0';
			lexer->result_symbol = HEREDOC_BODY;
			return true;
		}

		at_line_start = lexer->lookahead == '\n' || lexer->lookahead == '\r';
		lexer->advance(lexer, false);
		lexer->mark_end(lexer);
	}

	lexer->result_symbol = HEREDOC_BODY;
	scanner->delimiter_len = 0;
	scanner->delimiter[0] = '\0';
	return true;
}

bool tree_sitter_rcsh_external_scanner_scan(void *payload, TSLexer *lexer, const bool *valid_symbols) {
	Scanner *scanner = payload;

	if (valid_symbols[HEREDOC_MARKER])
		return scan_marker(lexer, scanner);

	if (valid_symbols[HEREDOC_BODY])
		return scan_body(lexer, scanner);

	return false;
}
