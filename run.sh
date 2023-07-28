#!/bin/sh
printf "test
"
set -xe
cc -g -o lex lex.c
cat run.sh | ./lex
# "test" "test\"" "test\\" "test"
# '' 't' '\\' '\'' '"' '\\' '@' ' '
# test. test test.test .
# 1 13 100. 3.14 127.0.0.1 3.14...