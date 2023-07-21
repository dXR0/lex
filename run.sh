#!/bin/sh
set -xe
cc -g -o lex lex.c
cat run.sh | ./lex
# "test" "test\"" "test\\" "test"