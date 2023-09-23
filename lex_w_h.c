#include "./lex.h"

int main(int argc, char **argv) {
	char *prog_name = shift(&argc, &argv); // shift program name
	Token **tokens;
	size_t *token_count = calloc(1, sizeof(size_t *));
	int repl = 0;
	int exit_code = 0;
	float res = 0;
	if (argc > 0) {
		tokens = w_args(argc, argv, token_count);
	} else {
		struct stat stats;
		fstat(fileno(stdin), &stats);
		int stats_mode = stats.st_mode;
		if (S_ISFIFO(stats_mode)) {
			tokens = s_isfifo(token_count);
		} else if (S_ISCHR(stats_mode)) {
			repl = 1;
			while (1) {
				tokens  = s_ischr(token_count);
				goto calc;
loop:
			}		
		} else if (S_ISREG(stats_mode)) {
			tokens = s_isreg(token_count);
		} else {
			fputs("[ERROR]: unsupported filetype\n", stderr);
			return 1;
		}
	}

calc:
	printer(tokens, *token_count);
	freemy(tokens, *token_count);
	if (repl) {
		goto loop;
	}
	free(token_count);
	return 0;
}