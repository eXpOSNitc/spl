%{
#include "y.tab.h"
#include "data.h"
int linecount=1;
struct define
{
    char *name;
    int value;
    struct define *next;
};
struct tree
{
    char nodetype;        /*    +,-,*,/,%,=,<,>
                    ?-if statement,        I-ireturn,    L-load
                    S-store,    P-strcmp,    Y-strcpy,    w-while,
                    R-register  //value=0-15,  20-BP,  21-SP,  22-IP, 23-PTBR, 24-PTLR, 25-EFR
                    e-double equals,    l-lessthan or equals
                    g-greaterthan or equals        !-not equal
                    a-AND        o-OR        x-NOT
                    c-number,    i-identifier,
                    n-nonterminal
                    b-break        t-continue    m-addresing expr
                    h-halt        C-checkpoint    I-ireturn        
                    1-read    2-print    s-STRING 
                    3-inline
                    4-loada 5-storea
                        */
    char *name;
    int value;
    struct define *entry;
    struct tree *ptr1,*ptr2,*ptr3;
};

void create_node(char, char*, int);    //nodetype,name,value
%}

%%


"alias"     {
                create_node('1',NULL,-1);
                return ALIAS;
            }

"define"    {
                return DEFINE;
            }

"encrypt"   {
                return ENCRYPT;//TODO
            }

"breakpoint" {
                create_node('C',NULL,-1);
                return CHKPT;
            }

"inline"    {
                create_node('3',NULL,-1);
                return INLINE;
            }

"halt"      {
                create_node('h',NULL,-1);
                return HALT;
            }

"goto"      {
                create_node('h',NULL,-1);//TODO
                return GOTO;
            }

"call"      {
                create_node('h',NULL,-1);//TODO
                return CALL;
            }

"return"    {
                create_node('I',NULL,-1);//TODO
                return RETURN;
            }            

"ireturn"   {
                create_node('I',NULL,-1);
                return IRETURN;
            }

"backup"    {
                create_node('6',NULL,-1);
                return BACKUP;
            }

"restore"   {
                create_node('7',NULL,-1);
                return RESTORE;
            }

"read"      {    
                create_node('1',NULL,-1);
                return READ;
            }

"reada"     {    
                create_node('1',NULL,-1);//TODO
                return READA;
            }

"print"     {
                create_node('2',NULL,-1);
                return PRINT;
            }

"load"      {
                create_node('L',NULL,-1);
                return LOAD;
            }
            
"loada"     {
                create_node('4',NULL,-1);
                return LOADA;
            }

"store"     {
                create_node('S',NULL,-1);
                return STORE;
            }

"storea"    {
                create_node('5',NULL,-1);
                return STOREA;
            }

"while"     {
                create_node('w',NULL,-1);
                return WHILE;
            }

"do"        {
                return DO;
            }
    
"endwhile"  {
                return ENDWHILE;
            }

"break"     {
                create_node('b',NULL,-1);
                return BREAK;
            }    

"continue"  {
                create_node('t',NULL,-1);
                return CONTINUE;
            }            
                            
"if"        {
                create_node('?',NULL,-1);
                return IF;
            }    

"then"      {
                return THEN;
            }

"else"      {
                return ELSE;
            }

"endif"     {
                return ENDIF;        
            }
            
[R]([0-9])  {
                create_node('R',NULL,atoi(yytext+1) + R0);
                return REG;
            }    

(R1)([0-5]) {
                create_node('R',NULL,atoi(yytext+1) + R0);
                return REG;
            }
            
[P]([0-3])  {
                create_node('R',NULL,atoi(yytext+1) + P0);
                return REG;
            }        

"BP"        {
                create_node('R',NULL,BP_REG);
                return REG;
            }

"SP"        {
                create_node('R',NULL,SP_REG);
                return REG;
            }

"IP"        {
                create_node('R',NULL,IP_REG);
                return REG;
            }

"PTBR"      {
                create_node('R',NULL,PTBR_REG);
                return REG;
            }

"PTLR"      {
                create_node('R',NULL,PTLR_REG);
                return REG;
            }

"EIP"       {
                create_node('R',NULL,EIP_REG);
                return REG;
            }

"EC"        {
                create_node('R',NULL,EC_REG);
                return REG;
            }
            
"EPN"       {
                create_node('R',NULL,EPN_REG);
                return REG;
            }

"EMA"       {
                create_node('R',NULL,EMA_REG);
                return REG;
            }
        
(==)        {
                create_node('e',NULL,-1);
                return RELOP;
            }
            
(<=)        {
                create_node('l',NULL,-1);
                return RELOP;
            }
            
(>=)        {
                create_node('g',NULL,-1);
                return RELOP;
            }
            
"!="        {
                create_node('!',NULL,-1);
                return RELOP;
            }

            
"&&"        {
                create_node('a',NULL,-1);
                return LOGOP;
            }

"||"        {
                create_node('o',NULL,-1);
                return LOGOP;
            }
            
"!"         {
                create_node('x',NULL,-1);
                return NOTOP;
            }

[+-]        {
                create_node(*yytext,NULL,-1);
                return ARITHOP1;            
            }
            
[*/%]       {
                create_node(*yytext,NULL,-1);
                return ARITHOP2;            
            }
            
[<>]        {
                create_node(*yytext,NULL,-1);
                return RELOP;
            }

"="         {
                create_node('=',NULL,-1);
                return ASSIGNOP;
            }
                        
[0-9]+      {
                create_node('c',NULL,atoi(yytext));
                return NUM;
            }

[\"][^\"]*[\"]        {    
                create_node('s',yytext,-1);
                return STRING;
            }
                    
[a-zA-Z][a-zA-Z0-9_]*    {
                create_node('i',yytext,-1);
                return ID;
            }    

[(),:;\[\]\{\}&]        {    
                return *yytext;
            }

[/][/].*\n        {
                linecount++;
            }

[\n]        {    
                linecount++;
            }

[ \t]       {
            }

.           {
            }

%%
void create_node(char nodetype, char *name, int value)
{    
    yylval.n=malloc(sizeof(struct tree));
    yylval.n->nodetype=nodetype;
    if(name!=NULL)
        yylval.n->name=strdup(name);
    yylval.n->value=value;
    yylval.n->entry=NULL;
    yylval.n->ptr1=NULL;
    yylval.n->ptr2=NULL;
    yylval.n->ptr3=NULL;
}
