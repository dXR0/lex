#include <stdio.h>
#include "./lex.h"

int main(int argc, char **argv) {
	shift(&argc, &argv); // program name
	const char *filename = shift(&argc, &argv);
	
	size_t token_count = 0;
	Token **tokens = file((char *)filename, &token_count);
	printer(tokens, token_count);

	return 0;
}
