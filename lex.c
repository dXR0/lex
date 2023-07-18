#include <stdio.h>
#include <sys/stat.h>
#include <stdlib.h>

#define SIZE 256

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

// TODO: 
int lex(char *buf, size_t size)
{
	printf("%s\nsize: %d\n", buf, size);
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
