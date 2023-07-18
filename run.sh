#!/bin/sh
set -xe
cc -o lex lex.c
cat run.sh | ./lex