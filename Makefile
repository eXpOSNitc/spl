#Our compiler.
CC = gcc
CFLAGS = -g
LEX = lex
YACC = yacc

#Default rule.

default: spl

spl: lex.yy.o y.tab.o node.o data.o spl.o
	$(CC) $(CFLAGS) -o spl  lex.yy.o y.tab.o node.o data.o spl.o 

node.o: node.c node.h
	$(CC) $(CFLAGS) -c node.c

data.o: data.c data.h
	$(CC) $(CFLAGS) -c data.c

spl.o: spl.c spl.h data.h node.h
	$(CC) $(CFLAGS) -c spl.c

lex.yy.o: lex.yy.c y.tab.c
	$(CC) $(CFLAGS) -c lex.yy.c

y.tab.o: y.tab.c y.tab.h
	$(CC) $(CFLAGS) -c y.tab.c

lex.yy.c: spl.l
	$(LEX) spl.l

y.tab.c: spl.y
	$(YACC) -d spl.y

clean:
	$(RM) spl *.o lex.yy.c y.tab.h y.tab.c
