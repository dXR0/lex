## lex

Non-specific lexer in C.

Now contains a header-only implementation for tokenizing input.
The intent is to enable copy-pasting the header file to different project and then modifying it to suit the tokenization needs of that project.

### How to

```sh
# compile `lex`
gcc -o lex lex.c

# Pipe file to `lex`
./lex < run.sh

# Pipe file to `lex`
cat run.sh | ./lex

# Use `lex` as REPL
./lex
```

### How to w/ header file

The functionality is the same as `lex.c`.
The only difference is that the tokenizing functionality is taken from `lex.h`.
To see how to use the header file, either check `lex_w_h.c` or [my calc project](https://github.com/dXR0/calc.git).

```sh
# compile `lex_w_h.c`
gcc -o lex lex_w_h.c

# Pipe file to `lex`
./lex < run.sh

# Pipe file to `lex`
cat run.sh | ./lex

# Use `lex` as REPL
./lex
```

### Author

Meelis Utt