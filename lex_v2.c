#pragma once

#define LEX_IMPLEMENTATION // TODO: rm

#ifndef TOOLBOXC_H_
#define TOOLBOXC_H_

#include <stdio.h>

char *shift(int *argc, char ***argv);
size_t mystrlen(char *s1);
int mystrcomp(char *s1, char *s2);

#endif // TOOLBOXC_H_

#ifndef TOOLBOXC_IMPLEMENTATION
#define TOOLBOXC_IMPLEMENTATION

#include <stdio.h>

char *shift(int *argc, char ***argv) {
	if (*argc <= 0) {
		return NULL;
	}
	char *result = **argv;
	*argc -= 1;
	*argv += 1;
	return result;
}

size_t mystrlen(char *s1) {
	char *s1c = s1;
	size_t size = 0;
	while (*(s1c++) != '\0') {
		++size;
	}
	return size;
}

int mystrcomp(char *s1, char *s2) {
	char *s1c = s1;
	char *s2c = s2;
	while (*s1c != '\0' && *s2c != '\0') {
		if (*s1c != *s2c) {
			break;
		}
		++s1c;
		++s2c;
	}
	return *s1c == *s2c;
}

#endif // TOOLBOXC_IMPLEMENTATION

#ifndef LEX_H_
#define LEX_H_

enum LEX_TOKEN_TYPE {
	LEX_SYMBOL = 0,
	LEX_WORD,
	LEX_INT,
	LEX_CHAR,
	LEX_STRING,
	LEX_TOKEN_COUNT
};

static const char *LEX_TOKEN_NAMES[] = {
	"SYMBOL",
	"WORD",
	"INT",
	"CHAR",
	"STRING"
};

typedef struct {
	enum LEX_TOKEN_TYPE t;
	char *v;
	int vlen;
	char *fname;
	int row;
	int col;
} lex_token;

void lex_free_tokens(lex_token **tokens, size_t n);
lex_token **tokenize(FILE *stream, size_t *token_counter);


#endif // LEX_H_

#ifdef LEX_IMPLEMENTATION

#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>

#define LEX_SIZE 256
char *LEX_FNAME = "stdin";


void lex_free_tokens(lex_token **tokens, size_t n) {
	for (int i=0; i<n; ++i) {
		free(tokens[i]->v);
		free(tokens[i]);
	}
	free(tokens);
}

char *bufferize(FILE *stream, size_t *buf_counter) {
	struct stat stats;
	fstat(fileno(stream), &stats);
	int stats_mode = stats.st_mode;
	char *buf;
	if (S_ISREG(stats_mode) || S_ISFIFO(stats_mode)) {
		size_t size = LEX_SIZE;
		buf = calloc(LEX_SIZE, sizeof(char));
		char c;
		size_t i = 0;
		while ((c = fgetc(stream)) != EOF) {
			buf[i] = c;
			++i;
			if (i >= size) {
				size *= 2;
				buf = realloc(buf, size);
			}
		}
		*buf_counter = i;
		buf = realloc(buf, i);
	} else {
		fprintf(stderr, "%s:%d: [ERROR]: bufferizing failed: unsupported stream provided to tokenizer\n", __FILE__, __LINE__);
		exit(1);
	}
	return buf;		
}

void string(lex_token *token) {
		printf("%s:%d:%d %s(%s)(%d)\n", token->fname, token->row, token->col, LEX_TOKEN_NAMES[token->t], token->v, token->vlen);
}

void print(lex_token **token, int token_count) {
	for (int i=0; i< token_count; ++i) {
		string(token[i]);
	}
}

lex_token **tokenize(FILE *stream, size_t *token_counter) {
	if (LEX_TOKEN_COUNT != sizeof(LEX_TOKEN_NAMES)/sizeof(*LEX_TOKEN_NAMES)) {
		return NULL;
	}
	size_t buf_counter = 0;
	char *buf = bufferize(stream, &buf_counter);
	if (buf == NULL) {
		fprintf(stderr, "%s:%d: [ERROR]: tokenizing failed: bufferizer returned NULL buffer\n",__FILE__, __LINE__);
		exit(1);
	} 

	int row = 1;
	int col = 1;
	size_t tok_count = 0;
	size_t str_size = LEX_SIZE;
	lex_token **tokens = calloc(buf_counter, sizeof(lex_token *));
	for (int i = 0; i<buf_counter; ++i) {
		lex_token *new = calloc(1, sizeof(lex_token));
		char *val = calloc(str_size, sizeof(char));
		new->fname = LEX_FNAME;
		new->row = row;
		new->col = col;
		char b_i = buf[i];
		int vlen = 0;
		if (b_i == '"') { // handle string
			new->t = LEX_STRING;
			++i;
			++col;
			int j = 0;
			int escaped = 0;
			while (i < buf_counter && ((b_i = buf[i]) != '"' || b_i == '"' && escaped)) {
				val[j] = b_i;
				++j;
				++i;
				++col;
				if (j >= str_size) {
					str_size *= 2;
					val = realloc(val, str_size);
				}
				int is_slash = b_i == '\\';
				escaped = is_slash * (is_slash ^ escaped);
			}
			val = realloc(val, j);
			vlen = j;
		} else if (b_i == '\'') { // handle char
			new->t = LEX_CHAR;
			++i;
			++col;
			int j = 0;
			int escaped = 0;
			while (i < buf_counter && ((b_i = buf[i]) != '\'' || b_i == '\'' && escaped)) {
				val[j] = b_i;
				++j;
				++i;
				++col;
				if (j >= str_size) {
					str_size *= 2;
					val = realloc(val, str_size);
				}
				int is_slash = b_i == '\\';
				escaped = is_slash * (is_slash ^ escaped);
			}
			val = realloc(val, j);
			vlen = j;
		} else if (b_i >= 'A' && b_i <= 'Z' || b_i >= 'a' && b_i <= 'z') { // handle word
		} else if (b_i >= '0' && b_i <= '9') { // handle int
			new->t = LEX_INT;
			++i;
			++col;
			val[0] = b_i;
			int j = 1;
			while ((b_i = buf[i]) >= '0' && b_i <= '9') {
				val[j] = b_i;
				++i;
				++col;
				++j;
			}
			val = realloc(val, j);
			vlen = j;
		} else { // handle symbol
		}
		new->v = val;
		new->vlen = vlen;
		tokens[tok_count] = new;
		++tok_count;
	}
	print(tokens, tok_count);
	free(buf);
}

#endif // LEX_IMPLEMENTATION

int main(int argc, char **argv) {
	size_t token_counter = 0;
	char *fname = "test.txt";
	FILE *fptr = fopen(fname, "r");
	LEX_FNAME=fname;
	tokenize(fptr, &token_counter);
	fclose(fptr);
}

