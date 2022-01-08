%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
typedef struct node {
	char *token;
	struct node *left;
	struct node *right;
} node;
node *nodeMaker(char *token, node *left, node *right);
void printtree(struct node *tree, int tab);
#define YYSTYPE struct node *	
int yylex();
int yyerror(char*);
%}
%token BOOL 
%token CHAR 
%token STRING
%token INT 
%token REAL 
%token IF 
%token WHILE 
%token FOR 
%token VAR 
%token RETURN 
%token NONE 
%token VOID 
%token DO 
%token COMMA 
%token ABS 
%token '[' 
%token ']' 
%token '&'
%token BOOLVAR CHARVAR INTVAR REALVAR STRINGVAR 
%token COMMENT
%token MAIN 
%token IDENT
%token ';'
%token ASSIGN 
%token GREATER GREATEREQ LESS LESSEQ EQ NOTEQ AND OR NOT
%token PLUS MINUS
%token STAR DIV
%token '{' '(' ')' '}'
%nonassoc IF
%nonassoc ELSE
%start s
%%
s: START {printtree($1, 0);};
START: CODE {$$ = nodeMaker ("CODE",$1,NULL); }
CODE: CODE FUNC {$$ = nodeMaker ("",$1,$2); }
      |FUNC {$$ = nodeMaker ("",$1,NULL); }
FUNC: TYPE FUNCDEC CMPSTATEMENT {$$ = nodeMaker ("FUNC",$1,nodeMaker("",$2,nodeMaker("BODY",$3,NULL))); }
	

TYPE: VOID {$$ = nodeMaker ( "VOID",NULL,NULL); }
     |BOOL {$$ = nodeMaker ( "BOOL",NULL,NULL); }
     |CHAR {$$ = nodeMaker ( "CHAR",NULL,NULL); }
     |INT {$$ = nodeMaker ( "INT",NULL,NULL); }
     |REAL {$$ = nodeMaker ( "REAL",NULL,NULL); }
     |STRING{$$ = nodeMaker ( "CHAR POINTER",NULL,NULL); }
     |CHAR STAR {$$ = nodeMaker ( "CHAR POINTER",NULL,NULL); }
     |REAL STAR {$$ = nodeMaker ( "REAL POINTER",NULL,NULL); }
     |INT STAR {$$ = nodeMaker ( "INT POINTER",NULL,NULL); };

FUNCDEC: IDENT '(' PARAMSLIST ')' {$$ = nodeMaker ("",$1,nodeMaker("PARAMETERS",$3,NULL)); }
	|IDENT '(' ')' {$$ = nodeMaker ("",nodeMaker($1->token,NULL,NULL),NULL); };

PARAMSLIST: PARAMSDEC {$$ = nodeMaker ("",$1,NULL); }| PARAMSLIST ';' PARAMSDEC {$$ = nodeMaker ("",$1,$3); };
PARAMSDEC: TYPE IDENTLIST {$$ = nodeMaker ("",$1,$2); };

VARDECLIST: VARDECLIST VARDEC {$$ = nodeMaker ("",$1,$2); }
		|VARDEC {$$ = nodeMaker ("",$1,NULL); }
VARDEC : VAR TYPE IDENTLIST ';' {$$ = nodeMaker ("",$2,$3); }
	| STRING ARRIDLIST ASSIGN STRINGVAR ';'{$$ = nodeMaker ("",nodeMaker("",nodeMaker("STRING",NULL,NULL),$2),nodeMaker("ASSIGN",$4,NULL)); };
	
ARRIDLIST: ARRIDLIST COMMA ARRID | ARRID
ARRID: IDENT '[' INTVAR ']'

IDENTLIST: IDENT {$$ = nodeMaker ("",$1,NULL); }
	| IDENTLIST COMMA IDENT{$$ = nodeMaker ("",$1,$3); };

CMPSTATEMENT: '{' '}' {$$ = nodeMaker ("{EMPTY}",NULL,NULL); }
	| '{' STATEMENTLIST '}' {$$ = nodeMaker ("{",$2,nodeMaker("}",NULL,NULL)); }
	| '{' VARDECLIST '}' {$$ = nodeMaker ("{",nodeMaker("VARDEC",$1,NULL),nodeMaker("}",NULL,NULL)); }
	| '{' VARDECLIST STATEMENTLIST '}'  {$$ = nodeMaker ("{",nodeMaker("VARDEC",$2,NULL),$3); };
	
STATEMENTLIST:STATEMENT{$$ = nodeMaker ("",$1,NULL); } | STATEMENTLIST STATEMENT{$$ = nodeMaker ("",$1,$2); };
STATEMENT:CMPSTATEMENT {$$ = nodeMaker ("",$1,NULL); }
	| EXPRSTATEMENT {$$ = nodeMaker ("",$1,NULL); }
	| SELECTSTATEMENT {$$ = nodeMaker ("",$1,NULL); }
	| ITRSTATEMENT {$$ = nodeMaker ("",$1,NULL); }
	| JMPSTATEMENT {$$ = nodeMaker ("",$1,NULL); };

EXPRSTATEMENT:';'
		|EXPR ';' {$$ =nodeMaker("",$1,NULL);};
EXPR:ASSIGNEXPR {$$ =nodeMaker("",$1,NULL);}
	| EXPR COMMA ASSIGNEXPR {$$ =nodeMaker("",$1,$3);};

ASSIGNEXPR:CONDITIONEXPR {$$ =nodeMaker("",$1,NULL);}
	   | PRIMARYEXPR ASSIGN ASSIGNEXPR {$$ =nodeMaker("ASSIGN",$1,nodeMaker(" ",NULL,$3));}  
	   | PRIMARYEXPR  '[' INTVAR ']' ASSIGN ASSIGNEXPR {$$ =nodeMaker("ASSIGN",$1,nodeMaker(" ",NULL,$3));}  ;;


CONDITIONEXPR:OREXPR {$$ =nodeMaker("",$1,NULL);};

OREXPR:ANDEXPR {$$ =nodeMaker("",$1,NULL);}
     | OREXPR OR ANDEXPR {$$ =nodeMaker("OR",$1,$3);};

ANDEXPR: EQEXPR {$$ =nodeMaker("",$1,NULL);} 
	| ANDEXPR AND EQEXPR {$$ =nodeMaker("AND",$1,$3);};
	
EQEXPR:RELATIONEXPR {$$ =nodeMaker("",$1,NULL);} 
	| EQEXPR EQ RELATIONEXPR {$$ =nodeMaker("EQUAL",$1,$3);} 
	| EQEXPR NOTEQ RELATIONEXPR {$$ =nodeMaker("NOT EQUAL",$1,$3);};

RELATIONEXPR:ADDITIVEEXPR {$$ =nodeMaker("",$1,NULL);}
	| RELATIONEXPR GREATER ADDITIVEEXPR {$$ =nodeMaker("",$1,nodeMaker("",nodeMaker(">",NULL,NULL),$3));}  
	| RELATIONEXPR LESS ADDITIVEEXPR  {$$ =nodeMaker("",$1,nodeMaker("",nodeMaker("<",NULL,NULL),$3));}  
	| RELATIONEXPR GREATEREQ ADDITIVEEXPR  {$$ =nodeMaker("",nodeMaker("",$1,NULL),nodeMaker("",nodeMaker(">=",NULL,NULL),$3));}  
	| RELATIONEXPR LESSEQ ADDITIVEEXPR {$$ =nodeMaker("",nodeMaker("",$1,NULL),nodeMaker("",nodeMaker("<=",NULL,NULL),$3));}   ;

ADDITIVEEXPR :MULTIPLICATIVEEXPR {$$ =nodeMaker("",$1,NULL);} 
	     | ADDITIVEEXPR PLUS MULTIPLICATIVEEXPR {$$ =nodeMaker("",$1,nodeMaker("",nodeMaker("+",NULL,NULL),$3));}  
	     |ADDITIVEEXPR MINUS MULTIPLICATIVEEXPR {$$ =nodeMaker("",nodeMaker("",$1,NULL),nodeMaker("",nodeMaker("-",NULL,NULL),$3));}  ;

MULTIPLICATIVEEXPR:CASTEXPR {$$ =nodeMaker("",$1,NULL);}
			| MULTIPLICATIVEEXPR STAR CASTEXPR {$$ =nodeMaker("",nodeMaker("",$1,NULL),nodeMaker("",nodeMaker("*",NULL,NULL),$3));}  
			| MULTIPLICATIVEEXPR DIV CASTEXPR {$$ =nodeMaker("",nodeMaker("",$1,NULL),nodeMaker("",nodeMaker("/",NULL,NULL),$3));}  ;

CASTEXPR:UNARYEXPR {$$ =nodeMaker("",$1,NULL);}
	| '(' TYPE ')' CASTEXPR {$$ =nodeMaker("(",nodeMaker("",$2,nodeMaker(")",NULL,NULL)),$4);};

UNARYEXPR:POSTFIX {$$ =nodeMaker("",$1,NULL);}
	| UNARYOP UNARYEXPR {$$ =nodeMaker("",$1,$2);};
	
UNARYOP: '&' {$$ =nodeMaker("&",NULL,NULL);} 
	| STAR {$$ =nodeMaker("*",NULL,NULL);} 
	| PLUS {$$ =nodeMaker("+",NULL,NULL);} 
	| MINUS {$$ =nodeMaker("-",NULL,NULL);} 
	| NOT {$$ =nodeMaker("NOT",NULL,NULL);} ;
	
POSTFIX: PRIMARYEXPR{$$ =nodeMaker("",$1,NULL);} ;

PRIMARYEXPR:IDENT {$$ =nodeMaker($1->token,NULL,NULL);}   
		| STRINGVAR {$$ =nodeMaker($1->token,NULL,NULL);}
		| REALVAR {$$ =nodeMaker($1->token,NULL,NULL);}
		| CHARVAR {$$ =nodeMaker($1->token,NULL,NULL);}
		| INTVAR {$$ =nodeMaker($1->token,NULL,NULL);}
		|BOOLVAR {$$ =nodeMaker($1->token,NULL,NULL);};

SELECTSTATEMENT:IF '(' EXPR ')' STATEMENT %prec IF {$$ = nodeMaker ("",nodeMaker("IF",$3,$5),NULL); }
		|IF '(' EXPR ')' STATEMENT  ELSE STATEMENT {$$ = nodeMaker ("",nodeMaker("IF",$3,$5),nodeMaker("ELSE",$7,NULL)); };

ITRSTATEMENT: WHILE '('EXPR ')' STATEMENT {$$ = nodeMaker ("WHILE",$3,$5); }
		| DO STATEMENT WHILE '(' EXPR ')' ';' {$$ = nodeMaker ("",nodeMaker("DO",$2,NULL),nodeMaker("WHILE",$5,NULL)); } 
	| FOR '(' EXPRSTATEMENT EXPRSTATEMENT ')' STATEMENT
	|FOR '(' EXPRSTATEMENT EXPRSTATEMENT EXPR ')' STATEMENT ;
		
JMPSTATEMENT: RETURN ';' {$$ = nodeMaker ("RETURN EMPTY",NULL,NULL); }
	| RETURN EXPR ';' {$$ = nodeMaker ("RETURN",$2,NULL); };

%%
#include "lex.yy.c"
int main(){
    return yyparse();
}

node *nodeMaker    (char *token, node *left, node *right){
    node *newnode = (node*)malloc (sizeof(node));
    char    *newstr = (char*)malloc (sizeof(token)+1);
    strcpy (newstr, token);
    newnode -> left = left;
    newnode -> right = right;
    newnode -> token = newstr;
    return newnode;
}

void printtree (node *tree, int tab){
    int i; 
    if(strcmp(tree->token,"")){
    for (i = 0; i< tab; i++)
        printf ("\t");
    char* token = tree->token;
    printf ("%s\n", token);
    }else tab-=1;
    if (tree -> left)
        printtree (tree-> left, tab + 1); 
    if (tree -> right)
        printtree (tree-> right, tab + 1 ); 
}

int yyerror(char* s){
    printf ("%s: at line %d found token [%s]\n", s, count, yytext);
    return 0;
}
