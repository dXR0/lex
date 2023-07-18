#include <stdio.h>
#include <sys/stat.h>
#include <stdlib.h>

#define SIZE 256
#define PRINT_WHITESPACE 0

// borrowed from tsoding
const char *shift(int *argc, char ***argv)
{
	if (*argc <= 0) {
		return NULL;
	}
	const char *result = *argv[0];
	*argc -= 1;
	*argv += 1;
	return result;
}

typedef enum {
	URL= -7,
	NUMBER = -6,
	FLOAT= -5,
	INT = -4,
	STRING = -3,
	CHAR = -2,
	WORD = -1,
	//
	TILDE = '~',
	TICK = '`',
	EXCLAIM = '!',
	AT = '@',
	DQUOTE = '"',
	QUOTE = '\'',
	HASH = '#',
	DOLLAR = '$',
	PERCENT = '%',
	CARET = '^',
	AMPER = '&',
	STAR = '*',
	SLASH = '/',
	BSLASH = '\\',
	CURLYL = '{',
	CURLYR = '}',
	PARENL = '(',
	PARENR = ')',
	BRACKETL = '[',
	BRACKETR = ']',
	ANGLEL = '<',
	ANGLER = '>',
	EQUAL = '=',
	DASH = '-',
	UNDERSCORE = '_',
	PLUS = '+',
	QUESTION = '?',
	SCOLON = ';',
	COLON = ':',
	PIPE = '|',
	SPACE = ' ',
	TAB = '\t',
	NEWLINE = '\n',
	DOT = '.',
	COMMA = ',',
} TOKEN_NAME;

typedef struct {
	TOKEN_NAME t;
	char *v;
} Token;

void to_string(Token *token){
	switch (token->t) {
		case WORD:
			printf("WORD(%s)\n", token->v);
			break;
		case CHAR:
			printf("CHAR(%s)\n", token->v);
			break;
		case STRING:
			printf("STRING(%s)\n", token->v);
			break;
		case INT:
			printf("INT(%s)\n",token->v);
			break;
		case FLOAT:
			printf("FLOAT(%s)\n",token->v);
			break;
		case NUMBER:
			printf("NUMBER(%s)\n", token->v);
			break;
		case URL:
			printf("URL(%s)\n", token->v);
			break;
		case TAB:
			if (PRINT_WHITESPACE) printf("SYMBOL(\\t)\n");
			break;
		case NEWLINE:
			if (PRINT_WHITESPACE) printf("SYMBOL(\\n)\n");
			break;
		case SPACE:
			if (PRINT_WHITESPACE) printf("SYMBOL(' ')\n");
			break;
		default:
			printf("SYMBOL(%s)\n", token->v);
	}
}

void printer(Token **tokens, size_t n)
{
	for (int i=0;i<n;i++) {
		to_string(tokens[i]);
	}
}

void freemy(Token **tokens, size_t n)
{
	for (int i=0; i<n; ++i) {
		free(tokens[i]->v);
		free(tokens[i]);
	}
	free(tokens);
}

// TODO: 
int lex(char *buf, size_t size)
{
	printf("%s\nsize: %d\n", buf, size);
	if (size == 0) {
		return 0;
	}

	Token **tokens = calloc(size, sizeof(Token *));
	for (int i=0; i< size; ++i) {
		Token *new = calloc(1, sizeof(Token));
		new->t = CHAR;
		char *val = calloc(1, sizeof(char));
		val[0] = buf[i];
		new->v = val;
		tokens[i] = new;
	}
	printer(tokens, size);
	freemy(tokens, size);
}

int s_isfifo() // piped in
{
	char *buf = calloc(SIZE, sizeof(char));
	char c;
	size_t i = 0;
	size_t size = SIZE;
	while ((c = fgetc(stdin)) != EOF) {
		buf[i] = c;
		++i;
		if (i >= size) {
			size *= 2;
			buf = realloc(buf, size);
		}
	}
	lex(buf, i);
	free(buf);
	return 0;
}

void my_memset(char *buf, int c, size_t n)
{
	for (int i=0; i<n; ++i) {
		buf[i] = c;
	}
}

int s_ischr() // REPL
{
	char *buf = calloc(SIZE, sizeof(char));
	size_t size = SIZE;
	char c;
	while (c != EOF) {
		size_t i = 0;
		while ((c = fgetc(stdin)) != '\n' && c != EOF) {
			buf[i] = c;
			++i;
			if (i >= size) {
				size *= 2;
				buf = realloc(buf, size);
			}
		}
		lex(buf, i);
		my_memset(buf, 0, size);
	}
	free(buf);
	return 0;

}

int s_isreg() // file directed as stdin, eg ./a.out < file
{
	fseek(stdin, 0, SEEK_END);
	int size = ftell(stdin);
	rewind(stdin);
	char *buf = calloc(size, sizeof(char));
	fread(buf, sizeof(char), size, stdin);
	lex(buf, size);
	free(buf);
	return 0;
}

int in()
{
	struct stat stats;
	fstat(fileno(stdin), &stats);
	int stats_mode = stats.st_mode;
	char c;
	if (S_ISFIFO(stats_mode)) {
		return s_isfifo();
	} else if (S_ISCHR(stats_mode)) {
		return s_ischr();
	} else if (S_ISREG(stats_mode)) {
		return s_isreg();
	} else {
		printf("unsupported filetype\n");
		return 1;
	}
	return 1;
}

int main(int argc, char **argv)
{
	shift(&argc, &argv); // remove program name from the arg list
	return in();
}
