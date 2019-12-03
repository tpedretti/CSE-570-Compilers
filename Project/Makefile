LEX = lex
YACC = yacc -d

CC = gcc

calc: y.tab.o lex.yy.o
	$(CC) -o calc y.tab.o lex.yy.o -ll -lm

lex.yy.o: lex.yy.c y.tab.h

y.tab.c y.tab.h: calc.yacc
	$(YACC) -v calc.yacc

lex.yy.c: calc.lex
	$(LEX) calc.lex

clean:
	-rm -f *.o lex.yy.o lex.yy.c *.tab.* calc *.output