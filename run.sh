#!/bin/sh
printf "test
"
set -xe
cc -g -o lex lex.c
cat run.sh | ./lex
# "test" "test\"" "test\\" "test"
# '' 't' '\\' '\'' '"' '\\' '@' ' '
# test. test test.test .