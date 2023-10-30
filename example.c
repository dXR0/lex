#define LEX_IMPLEMENTATION 
#include "./lex.h"

int main(int argc, char **argv) {
	size_t token_counter = 0;
	char *fname = "test.txt";
	FILE *fptr = fopen(fname, "r");
	LEX_FNAME=fname;
	lex_token **tokens = tokenize(fptr, &token_counter);
	fclose(fptr);
	print(tokens, token_counter);
	lex_free(tokens, token_counter);
}

