%{
#include <stdio.h>
#include "y.tab.h"

int c;
double d;
extern YYSTYPE yylval;
%}

%%

" ";


[a-z] {
  c = yytext[0];
  yylval.a = c - 'a';
  return(LETTER);
}

[0-9] {
  d = atof(yytext);
  yylval.a = d - 0;
  return(DIGIT);
}

[^a-z0-9\b] {
  c = yytext[0];
  return(c);
}

[0-9]+\.[0-9]+ {
  d = atof(yytext);
  yylval.a = d - 0;
  return(DIGIT);
}

QUIT {
  return QUIT;
}

SIN {
  return SIN;
}

COS {
  return COS;
}

TAN {
  return TAN;
}

SQRT {
  return SQRT;
}

EXP {
  return EXP;
}

LOG {
  return LOG;
}

LOG2 {
  return LOG2;
}