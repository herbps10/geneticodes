#gcc -o gene-expression src/gene-expression.c -std=c99 -lpthread

gcc -o gene-expression src/gene-expression.c -std=c99 -g -lpthread -pg # This line enables profiling & debugging
