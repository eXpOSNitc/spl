all:	spl
lex.yy.c:	spl.l data.h
		lex spl.l

y.tab.c:	spl.y spl.h	data.h
		yacc -d spl.y
spl:		lex.yy.c y.tab.c data.c
		gcc lex.yy.c y.tab.c data.c -ll -o spl
clean:
	rm -rf spl *~ y.* lex.* 
