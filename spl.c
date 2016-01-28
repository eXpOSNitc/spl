#include <stdlib.h>
#include <stdio.h>
#include "spl.h"

unsigned long temp_pos; //temporary lseek
int out_linecount=0; //no of lines of code generated
int addrBaseVal;    //Starting Address where the compiled code will be loaded
int flag_break=0;
int regcount=0;

FILE *fp;
                        //start labels
int labelcount=0;
struct label *root_label=NULL, *root_while=NULL;

struct define *root_define=NULL;
char alias_table[8][30];
struct define* lookup_constant(char *name);
int depth=0;

struct alias *root_alias=NULL;

void push_label()
{
    struct label *temp;
    temp=malloc(sizeof(struct label));
    temp->i=labelcount;
    temp->pos1=0;
    temp->pos2=0;
    bzero(temp->instr1,32);
    bzero(temp->instr2,32);
    temp->points=NULL;
    temp->next=root_label;    
    root_label=temp; 
    labelcount++;
}
int pop_label()
{
    int i;
    struct label *temp;
    temp=root_label;
    root_label=root_label->next;
    i=temp->i;
    free(temp);
    return i;
}
void push_while(int n)
{
    struct label *temp;
    temp=malloc(sizeof(struct label));
    temp->i=n;
    temp->pos1=0;
    temp->pos2=0;
    bzero(temp->instr1,32);
    bzero(temp->instr2,32);
    temp->points=NULL;
    temp->next=root_while;
    root_while=temp; 
}
void pop_while()
{
    struct label *temp;
    temp=root_while;
    root_while=root_while->next;
    free(temp);
}
void add_jmp_point(char instr[32])
{
    struct jmp_point *temp;
    temp=malloc(sizeof(struct jmp_point)); 
    fflush(fp);
    temp->pos = ftell(fp);
    strcpy(temp->instr,instr);
    temp->next = root_while->points;
    root_while->points = temp;    
}
void use_jmp_points(struct jmp_point *root)
{
    if(root == NULL)
        return;
    else
    {
        fflush(fp);
        temp_pos = ftell(fp);
        fseek(fp,root->pos,SEEK_SET);
        fprintf(fp,"%s %05d",root->instr,addrBaseVal + out_linecount*2);
        fseek(fp,temp_pos,SEEK_SET);
        use_jmp_points(root->next);
        free(root);
    }
}
                        ///end labels
                        ///start constants and aliasing

struct define* lookup_constant(char *name)
{
    struct define *temp=root_define;    
    while(temp!=NULL)
    {
        if(strcmp(name, temp->name)==0)
            return temp;
        temp=temp->next;
    }
    return NULL;
}
struct alias * lookup_alias(char *name)
{
    struct alias *temp=root_alias;
    while(temp!=NULL)
    {
        if(strcmp(temp->name, name)==0)
            return(temp);
        temp=temp->next;
    }
    return(NULL);
}
struct alias * lookup_alias_reg(int no)
{
    struct alias *temp=root_alias;
    while(temp!=NULL)
    {
        if(no==temp->no)
            return(temp);
        temp=temp->next;
    }
    return(NULL);
}
void push_alias(char *name, int no)
{
    struct alias *temp;        
    if(lookup_constant(name)!=NULL)
    {
        printf("\n%d: Alias name %s already used as symbolic contant!!\n", linecount, name);
        exit(0);
    }
    temp=lookup_alias(name);
    if(temp!=NULL && temp->depth == depth)
    {
        printf("\n%d: Alias name %s already used as in the current block!!\n", linecount, name);
        exit(0);
    }
    else
    {    temp=lookup_alias_reg(no);
        if(temp!=NULL && temp->depth==depth)
            strcpy(temp->name, name);
        else
        {
            temp=malloc(sizeof(struct alias));
            strcpy(temp->name, name);
            temp->no=no;
            temp->depth=depth;
            temp->next=root_alias;
            root_alias=temp;
        }        
    }
}
void pop_alias()
{
    struct alias *temp;
    temp=root_alias;
    while(temp!=NULL && temp->depth==depth)
    {
        root_alias=temp->next;
        free(temp);
        temp=root_alias;
    }
}
void insert_constant(char *name, int value)
{
    struct define * temp;
    temp=lookup_constant(name);
    if(temp==NULL)
    {
        temp=malloc(sizeof(struct define));
        strcpy(temp->name,name);
        temp->value=value;
        temp->next=root_define;
        root_define=temp;
    }
    else
    {
        printf("\n%d: Multiple Definition for Contant %s \n", linecount, name);
        exit(0);
    }
}      
void add_predefined_constants()
{
    struct define * temp;
    char name[15];
        
    bzero(name,15);
    strcpy(name, "INODE_TABLE");
    if(lookup_constant(name)==NULL)
        insert_constant(name, INODE_TABLE);
        
    bzero(name,15);
    strcpy(name, "DISK_FREE_LIST");
    if(lookup_constant(name)==NULL)
        insert_constant(name, DISK_FREE_LIST);
        
    bzero(name,15);
    strcpy(name, "ROOT_FILE");
    if(lookup_constant(name)==NULL)
        insert_constant(name, ROOT_FILE);
        
    bzero(name,15);
    strcpy(name, "USER_TABLE");
    if(lookup_constant(name)==NULL)
        insert_constant(name, USER_TABLE);
        
    bzero(name,15);
    strcpy(name, "PROCESS_TABLE");
    if(lookup_constant(name)==NULL)
        insert_constant(name, PROCESS_TABLE);
        
    bzero(name,15);
    strcpy(name, "FILE_TABLE");
    if(lookup_constant(name)==NULL)
        insert_constant(name, FILE_TABLE);
        
    bzero(name,15);
    strcpy(name, "SEMAPHORE_TABLE");
    if(lookup_constant(name)==NULL)
        insert_constant(name, SEMAPHORE_TABLE);
        
    bzero(name,15);
    strcpy(name, "DISK_STATUS_TABLE");
    if(lookup_constant(name)==NULL)
        insert_constant(name, DISK_STATUS_TABLE);
        
    bzero(name,15);
    strcpy(name, "BUFFER_TABLE");
    if(lookup_constant(name)==NULL)
        insert_constant(name, BUFFER_TABLE);
        
    bzero(name,15);
    strcpy(name, "SYSTEM_STATUS_TABLE");
    if(lookup_constant(name)==NULL)
        insert_constant(name, SYSTEM_STATUS_TABLE);
        
    bzero(name,15);
    strcpy(name, "TERMINAL_STATUS_TABLE");
    if(lookup_constant(name)==NULL)
        insert_constant(name, TERMINAL_STATUS_TABLE);
        
    bzero(name,15);
    strcpy(name, "MEMORY_FREE_LIST");
    if(lookup_constant(name)==NULL)
        insert_constant(name, MEMORY_FREE_LIST);
        
    bzero(name,15);
    strcpy(name, "EXCEPTION");
    if(lookup_constant(name)==NULL)
        insert_constant(name, EXCEPTION);
        
    bzero(name,15);
    strcpy(name, "TIMER");
    if(lookup_constant(name)==NULL)
        insert_constant(name, TIMER);
        
    bzero(name,15);
    strcpy(name, "DISK");
    if(lookup_constant(name)==NULL)
        insert_constant(name, DISK);
        
    bzero(name,15);
    strcpy(name, "CONSOLE");
    if(lookup_constant(name)==NULL)
        insert_constant(name, CONSOLE);
        
    bzero(name,15);
    strcpy(name, "INT_4");
    if(lookup_constant(name)==NULL)
        insert_constant(name, INT_4);
        
    bzero(name,15);
    strcpy(name, "INT_5");
    if(lookup_constant(name)==NULL)
        insert_constant(name, INT_5);
        
    bzero(name,15);
    strcpy(name, "INT_6");
    if(lookup_constant(name)==NULL)
        insert_constant(name, INT_6);
        
    bzero(name,15);
    strcpy(name, "INT_7");
    if(lookup_constant(name)==NULL)
        insert_constant(name, INT_7);
        
    bzero(name,15);
    strcpy(name, "INT_8");
    if(lookup_constant(name)==NULL)
        insert_constant(name, INT_8);
        
    bzero(name,15);
    strcpy(name, "INT_9");
    if(lookup_constant(name)==NULL)
        insert_constant(name, INT_9);
        
    bzero(name,15);
    strcpy(name, "INT_10");
    if(lookup_constant(name)==NULL)
        insert_constant(name, INT_10);
        
    bzero(name,15);
    strcpy(name, "INT_11");
    if(lookup_constant(name)==NULL)
        insert_constant(name, INT_11);
        
    bzero(name,15);
    strcpy(name, "INT_12");
    if(lookup_constant(name)==NULL)
        insert_constant(name, INT_12);
        
    bzero(name,15);
    strcpy(name, "INT_13");
    if(lookup_constant(name)==NULL)
        insert_constant(name, INT_13);
        
    bzero(name,15);
    strcpy(name, "INT_14");
    if(lookup_constant(name)==NULL)
        insert_constant(name, INT_14);
        
    bzero(name,15);
    strcpy(name, "INT_15");
    if(lookup_constant(name)==NULL)
        insert_constant(name, INT_15);
        
    bzero(name,15);
    strcpy(name, "INT_16");
    if(lookup_constant(name)==NULL)
        insert_constant(name, INT_16);
        
    bzero(name,15);
    strcpy(name, "INT_17");
    if(lookup_constant(name)==NULL)
        insert_constant(name, INT_17);
        
    bzero(name,15);
    strcpy(name, "INT_18");
    if(lookup_constant(name)==NULL)
        insert_constant(name, INT_18);
        
    bzero(name,15);
    strcpy(name, "MOD_0");
    if(lookup_constant(name)==NULL)
        insert_constant(name, MOD_0);
        
    bzero(name,15);
    strcpy(name, "MOD_1");
    if(lookup_constant(name)==NULL)
        insert_constant(name, MOD_1);
        
    bzero(name,15);
    strcpy(name, "MOD_2");
    if(lookup_constant(name)==NULL)
        insert_constant(name, MOD_2);
        
    bzero(name,15);
    strcpy(name, "MOD_3");
    if(lookup_constant(name)==NULL)
        insert_constant(name, MOD_3);
        
    bzero(name,15);
    strcpy(name, "MOD_4");
    if(lookup_constant(name)==NULL)
        insert_constant(name, MOD_4);
        
    bzero(name,15);
    strcpy(name, "MOD_5");
    if(lookup_constant(name)==NULL)
        insert_constant(name, MOD_5);
        
    bzero(name,15);
    strcpy(name, "MOD_6");
    if(lookup_constant(name)==NULL)
        insert_constant(name, MOD_6);
        
    bzero(name,15);
    strcpy(name, "MOD_7");
    if(lookup_constant(name)==NULL)
        insert_constant(name, MOD_7);
        
    bzero(name,15);
    strcpy(name, "DISK_SWAP_AREA");
    if(lookup_constant(name)==NULL)
        insert_constant(name, DISK_SWAP_AREA);
}

struct tree * substitute_id(struct tree *id)
{
    struct define *temp;
    struct alias *temp2;
    temp=lookup_constant(id->name);
    if(temp!=NULL)
    {
        id->nodetype=NODE_NUM;
        id->name=NULL;
        id->value=temp->value;
        return(id);
    }
    temp2=lookup_alias(id->name);
    if(temp2==NULL)
    {
        printf("\n%d: Unknown identifier %s used!!\n", linecount, id->name);
        exit(0);
    }
    id->nodetype=NODE_REG;
    id->name=NULL;
    id->value=temp2->no;
    return(id);
}
                            ///end of constants and alias
                            ///start tree create fns
struct tree * create_nonterm_node(char *name, struct tree *a, struct tree *b)
{
    struct tree *temp=malloc(sizeof(struct tree));
    temp->nodetype=NODE_NONTERM;
    temp->name=name;
    temp->entry=NULL;
    temp->ptr1=a;
    temp->ptr2=b;
    temp->ptr3=NULL;
    return temp;
}
struct tree * create_tree(struct tree *a, struct tree *b, struct tree *c, struct tree *d)
{    
    a->ptr1=b;
    a->ptr2=c;
    a->ptr3=d;
    
    return a;
}
                            ///end tree create fns
void getreg(struct tree *root, char reg[])
{
    if(root->value >= R0 && root->value <= R15)
        sprintf(reg, "R%d", root->value - R0);
    else if(root->value >= P0 && root->value <= P3)
        sprintf(reg, "P%d", root->value - P0);
    else if(root->value == BP_REG)
        sprintf(reg, "BP");
    else if(root->value == SP_REG)
        sprintf(reg, "SP");
    else if(root->value == IP_REG)
        sprintf(reg, "IP");
    else if(root->value == PTBR_REG)
        sprintf(reg, "PTBR");
    else if(root->value == PTLR_REG)
        sprintf(reg, "PTLR");
    else if(root->value == EIP_REG)
        sprintf(reg, "EIP");
    else if(root->value == EPN_REG)
        sprintf(reg, "EPN");
    else if(root->value == EC_REG)
        sprintf(reg, "EC");
    else if(root->value == EMA_REG)
        sprintf(reg, "EMA");
}

void codegen(struct tree * root)
{
    int n;
    char reg1[5], reg2[5];
    if(root==NULL)
        return;    
    switch(root->nodetype)
    {
        case NODE_LT:
            if(root->ptr1->nodetype==NODE_REG)
            {
                getreg(root->ptr1, reg1);
                if(root->ptr2->nodetype==NODE_REG)
                {
                    getreg(root->ptr2, reg2);
                    out_linecount+=2;
                    fprintf(fp, "MOV R%d,  %s\nLT R%d,  %s\n", C_REG_BASE + regcount,  reg1,  C_REG_BASE + regcount,  reg2);
                    regcount++;    
                }
                else
                {
                    codegen(root->ptr2);
                    out_linecount++;
                    fprintf(fp,  "GT R%d,  %s\n",  C_REG_BASE + regcount-1, reg1);
                }                    
            }
            else
            {
                codegen(root->ptr1);            
                if(root->ptr2->nodetype==NODE_REG)
                {
                    getreg(root->ptr2, reg2);
                    out_linecount++;
                    fprintf(fp, "LT R%d, %s\n", C_REG_BASE + regcount-1, reg2);    
                }
                else
                {
                    codegen(root->ptr2);
                    out_linecount++;
                    fprintf(fp, "LT R%d, R%d\n", C_REG_BASE + regcount-2, C_REG_BASE + regcount-1);
                    regcount--;
                }
            }            
            break;
        case NODE_GT:
            if(root->ptr1->nodetype==NODE_REG)
            {
                getreg(root->ptr1, reg1);
                if(root->ptr2->nodetype==NODE_REG)
                {
                    getreg(root->ptr2, reg2);
                    out_linecount+=2;
                    fprintf(fp, "MOV R%d, %s\nGT R%d, %s\n", C_REG_BASE + regcount, reg1, C_REG_BASE + regcount, reg2);
                    regcount++;    
                }
                else
                {
                    codegen(root->ptr2);
                    out_linecount++;
                    fprintf(fp, "LT R%d, %s\n", C_REG_BASE + regcount-1, reg1);
                }                    
            }
            else
            {
                codegen(root->ptr1);            
                if(root->ptr2->nodetype==NODE_REG)
                {
                    getreg(root->ptr2, reg2);
                    out_linecount++;
                    fprintf(fp, "GT R%d, %s\n", C_REG_BASE + regcount-1, reg2);    
                }
                else
                {
                    codegen(root->ptr2);
                    out_linecount++;
                    fprintf(fp, "GT R%d, R%d\n", C_REG_BASE + regcount-2, C_REG_BASE + regcount-1);
                    regcount--;
                }
            }            
            break;
        case NODE_EQ:        //double equals
            if(root->ptr1->nodetype==NODE_REG)
            {
                getreg(root->ptr1, reg1);
                if(root->ptr2->nodetype==NODE_REG)
                {
                    getreg(root->ptr2, reg2);
                    out_linecount+=2;
                    fprintf(fp, "MOV R%d, %s\nEQ R%d, %s\n", C_REG_BASE + regcount, reg1, C_REG_BASE + regcount, reg2);
                    regcount++;    
                }
                else
                {
                    codegen(root->ptr2);
                    out_linecount++;
                    fprintf(fp, "EQ R%d, %s\n", C_REG_BASE + regcount-1, reg1);
                }                    
            }
            else
            {
                codegen(root->ptr1);            
                if(root->ptr2->nodetype==NODE_REG)
                {
                    getreg(root->ptr2, reg2);
                    out_linecount++;
                    fprintf(fp, "EQ R%d, %s\n", C_REG_BASE + regcount-1, reg2);    
                }
                else
                {
                    codegen(root->ptr2);
                    out_linecount++;
                    fprintf(fp, "EQ R%d, R%d\n", C_REG_BASE + regcount-2, C_REG_BASE + regcount-1);
                    regcount--;
                }
            }            
            break;
        case NODE_LE:        //lessthan or equals
            if(root->ptr1->nodetype==NODE_REG)
            {
                getreg(root->ptr1, reg1);
                if(root->ptr2->nodetype==NODE_REG)
                {
                    getreg(root->ptr2, reg2);
                    out_linecount+=2;
                    fprintf(fp, "MOV R%d, %s\nLE R%d, %s\n", C_REG_BASE + regcount, reg1, C_REG_BASE + regcount, reg2);
                    regcount++;    
                }
                else
                {
                    codegen(root->ptr2);
                    out_linecount++;
                    fprintf(fp, "GE R%d, %s\n", C_REG_BASE + regcount-1, reg1);
                }                    
            }
            else
            {
                codegen(root->ptr1);            
                if(root->ptr2->nodetype==NODE_REG)
                {
                    getreg(root->ptr2, reg2);
                    out_linecount++;
                    fprintf(fp, "LE R%d, %s\n", C_REG_BASE + regcount-1, reg2);    
                }
                else
                {
                    codegen(root->ptr2);
                    out_linecount++;
                    fprintf(fp, "LE R%d, R%d\n", C_REG_BASE + regcount-2, C_REG_BASE + regcount-1);
                    regcount--;
                }
            }            
            break;
        case NODE_GE:        //greaterthan or equals    
            if(root->ptr1->nodetype==NODE_REG)
            {
                getreg(root->ptr1, reg1);
                if(root->ptr2->nodetype==NODE_REG)
                {
                    getreg(root->ptr2, reg2);
                    out_linecount+=2;
                    fprintf(fp, "MOV R%d, %s\nGE R%d, %s\n", C_REG_BASE + regcount, reg1, C_REG_BASE + regcount, reg2);
                    regcount++;    
                }
                else
                {
                    codegen(root->ptr2);
                    out_linecount++;
                    fprintf(fp, "LE R%d, %s\n", C_REG_BASE + regcount-1, reg1);
                }                    
            }
            else
            {
                codegen(root->ptr1);            
                if(root->ptr2->nodetype==NODE_REG)
                {
                    getreg(root->ptr2, reg2);
                    out_linecount++;
                    fprintf(fp, "GE R%d, %s\n", C_REG_BASE + regcount-1, reg2);    
                }
                else
                {
                    codegen(root->ptr2);
                    out_linecount++;
                    fprintf(fp, "GE R%d, R%d\n", C_REG_BASE + regcount-2, C_REG_BASE + regcount-1);
                    regcount--;
                }
            }            
            break;
        case NODE_NE:        //not equal
            if(root->ptr1->nodetype==NODE_REG)
            {
                getreg(root->ptr1, reg1);
                if(root->ptr2->nodetype==NODE_REG)
                {
                    getreg(root->ptr2, reg2);
                    out_linecount+=2;
                    fprintf(fp, "MOV R%d, %s\nNE R%d, %s\n", C_REG_BASE + regcount, reg1, C_REG_BASE + regcount, reg2);
                    regcount++;    
                }
                else
                {
                    codegen(root->ptr2);
                    out_linecount++;
                    fprintf(fp, "NE R%d, %s\n", C_REG_BASE + regcount-1, reg1);
                }                    
            }
            else
            {
                codegen(root->ptr1);            
                if(root->ptr2->nodetype==NODE_REG)
                {
                    getreg(root->ptr2, reg2);
                    out_linecount++;
                    fprintf(fp, "NE R%d, %s\n", C_REG_BASE + regcount-1, reg2);    
                }
                else
                {
                    codegen(root->ptr2);
                    out_linecount++;
                    fprintf(fp, "NE R%d, R%d\n", C_REG_BASE + regcount-2, C_REG_BASE + regcount-1);
                    regcount--;
                }
            }            
            break;
        case NODE_AND:    //AND operator
            if(root->ptr1->nodetype==NODE_REG)
            {
                getreg(root->ptr1, reg1);
                if(root->ptr2->nodetype==NODE_REG)
                {
                    getreg(root->ptr2, reg2);
                    out_linecount+=2;
                    fprintf(fp, "MOV R%d, %s\nMUL R%d, %s\n", C_REG_BASE + regcount, reg1, C_REG_BASE + regcount, reg2);
                    regcount++;    
                }
                else
                {
                    codegen(root->ptr2);
                    out_linecount++;
                    fprintf(fp, "MUL R%d, %s\n", C_REG_BASE + regcount-1, reg1);
                }                    
            }
            else
            {
                codegen(root->ptr1);            
                if(root->ptr2->nodetype==NODE_REG)
                {
                    getreg(root->ptr2, reg2);
                    out_linecount++;
                    fprintf(fp, "MUL R%d, %s\n", C_REG_BASE + regcount-1, reg2);    
                }
                else
                {
                    codegen(root->ptr2);
                    out_linecount++;
                    fprintf(fp, "MUL R%d, R%d\n", C_REG_BASE + regcount-2, C_REG_BASE + regcount-1);
                    regcount--;
                }
            }            
            break;
        case NODE_OR:    //OR operator
            if(root->ptr1->nodetype==NODE_REG)
            {
                getreg(root->ptr1, reg1);
                if(root->ptr2->nodetype==NODE_REG)
                {
                    getreg(root->ptr2, reg2);
                    out_linecount+=2;
                    fprintf(fp, "MOV R%d, %s\nADD R%d, %s\n", C_REG_BASE + regcount, reg1, C_REG_BASE + regcount, reg2);
                    regcount++;    
                }
                else
                {
                    codegen(root->ptr2);
                    out_linecount++;
                    fprintf(fp, "ADD R%d, %s\n", C_REG_BASE + regcount-1, reg1);
                }                    
            }
            else
            {
                codegen(root->ptr1);            
                if(root->ptr2->nodetype==NODE_REG)
                {
                    getreg(root->ptr2, reg2);
                    out_linecount++;
                    fprintf(fp, "ADD R%d, %s\n", C_REG_BASE + regcount-1, reg2);    
                }
                else
                {
                    codegen(root->ptr2);
                    out_linecount++;
                    fprintf(fp, "ADD R%d, R%d\n", C_REG_BASE + regcount-2, C_REG_BASE + regcount-1);
                    regcount--;
                }
            }            
            break;
        case NODE_NOT:    //NOT operator
            out_linecount++;
            fprintf(fp, "MOV R%d, 1\n", C_REG_BASE + regcount);
            regcount++;
            if(root->ptr1->nodetype==NODE_REG)
            {
                getreg(root->ptr1, reg1);
                out_linecount++;
                fprintf(fp, "SUB R%d, %s\n", C_REG_BASE + regcount-1, reg1);
            }
            else
            {
                codegen(root->ptr1);
                out_linecount++;
                fprintf(fp, "SUB R%d, R%d\n", C_REG_BASE + regcount-2, C_REG_BASE + regcount-1);
                regcount--;
            }
            break;        
        case NODE_STMTLIST:    //statement list
            codegen(root->ptr1);
            codegen(root->ptr2);            
            break;
        case NODE_ADD:
            if(root->ptr1->nodetype==NODE_REG)
            {
                getreg(root->ptr1, reg1);
                if(root->ptr2->nodetype==NODE_REG)
                {
                    getreg(root->ptr2, reg2);
                    out_linecount+=2;
                    fprintf(fp, "MOV R%d, %s\nADD R%d, %s\n", C_REG_BASE + regcount, reg1, C_REG_BASE + regcount, reg2);
                    regcount++;    
                }
                else if(root->ptr2->nodetype==NODE_NUM)
                {
                    out_linecount+=2;
                    fprintf(fp, "MOV R%d, %s\nADD R%d, %d\n", C_REG_BASE + regcount, reg1, C_REG_BASE + regcount, root->ptr2->value);
                    regcount++;    
                }
                else
                {
                    codegen(root->ptr2);
                    out_linecount++;
                    fprintf(fp, "ADD R%d, %s\n", C_REG_BASE + regcount-1, reg1);
                }                    
            }
            else
            {
                codegen(root->ptr1);            
                if(root->ptr2->nodetype==NODE_REG)
                {
                    getreg(root->ptr2, reg2);
                    out_linecount++;
                    fprintf(fp, "ADD R%d, %s\n", C_REG_BASE + regcount-1, reg2);    
                }
                else if(root->ptr2->nodetype==NODE_NUM)
                {
                    out_linecount++;
                    fprintf(fp, "ADD R%d, %d\n", C_REG_BASE + regcount-1, root->ptr2->value);    
                }
                else
                {
                    codegen(root->ptr2);
                    out_linecount++;
                    fprintf(fp, "ADD R%d, R%d\n", C_REG_BASE + regcount-2, C_REG_BASE + regcount-1);
                    regcount--;
                }
            }            
            break;
        case NODE_SUB:
            if(root->ptr1->nodetype==NODE_REG)
            {
                getreg(root->ptr1, reg1);
                if(root->ptr2->nodetype==NODE_REG)
                {
                    getreg(root->ptr2, reg2);
                    out_linecount+=2;
                    fprintf(fp, "MOV R%d, %s\nSUB R%d, %s\n", C_REG_BASE + regcount, reg1, C_REG_BASE + regcount, reg2);
                    regcount++;    
                }
                else if(root->ptr2->nodetype==NODE_NUM)
                {
                    out_linecount+=2;
                    fprintf(fp, "MOV R%d, %s\nSUB R%d, %d\n", C_REG_BASE + regcount, reg1, C_REG_BASE + regcount, root->ptr2->value);    
                    regcount++;    
                }
                else
                {
                    out_linecount++;
                    fprintf(fp, "MOV R%d, %s\n", C_REG_BASE + regcount, reg1);
                    regcount++;
                    codegen(root->ptr2);
                    out_linecount++;
                    fprintf(fp, "SUB R%d, R%d\n", C_REG_BASE + regcount-2, C_REG_BASE + regcount-1);
                    regcount--;
                }                    
            }
            else
            {
                codegen(root->ptr1);            
                if(root->ptr2->nodetype==NODE_REG)
                {
                    getreg(root->ptr2, reg2);
                    out_linecount++;
                    fprintf(fp, "SUB R%d, %s\n", C_REG_BASE + regcount-1, reg2);    
                }
                else if(root->ptr2->nodetype==NODE_NUM)
                {
                    out_linecount++;
                    fprintf(fp, "SUB R%d, %d\n", C_REG_BASE + regcount-1, root->ptr2->value);    
                }
                else
                {
                    codegen(root->ptr2);
                    out_linecount++;
                    fprintf(fp, "SUB R%d, R%d\n", C_REG_BASE + regcount-2, C_REG_BASE + regcount-1);
                    regcount--;
                }
            }            
            break;
        case NODE_MUL:
            if(root->ptr1->nodetype==NODE_REG)
            {
                getreg(root->ptr1, reg1);
                if(root->ptr2->nodetype==NODE_REG)
                {
                    getreg(root->ptr2, reg2);
                    out_linecount+=2;
                    fprintf(fp, "MOV R%d, %s\nMUL R%d, %s\n", C_REG_BASE + regcount, reg1, C_REG_BASE + regcount, reg2);
                    regcount++;    
                }
                else if(root->ptr2->nodetype==NODE_NUM)
                {
                    out_linecount+=2;
                    fprintf(fp, "MOV R%d, %s\nMUL R%d, %d\n", C_REG_BASE + regcount, reg1, C_REG_BASE + regcount, root->ptr2->value);
                    regcount++;        
                }
                else
                {
                    codegen(root->ptr2);
                    out_linecount++;
                    fprintf(fp, "MUL R%d, %s\n", C_REG_BASE + regcount-1, reg1);
                }                    
            }
            else
            {
                codegen(root->ptr1);            
                if(root->ptr2->nodetype==NODE_REG)
                {
                    getreg(root->ptr2, reg2);
                    out_linecount++;
                    fprintf(fp, "MUL R%d, %s\n", C_REG_BASE + regcount-1, reg2);    
                }
                else if(root->ptr2->nodetype==NODE_NUM)
                {
                    out_linecount++;
                    fprintf(fp, "MUL R%d, %d\n", C_REG_BASE + regcount-1, root->ptr2->value);    
                }
                else
                {
                    codegen(root->ptr2);
                    out_linecount++;
                    fprintf(fp, "MUL R%d, R%d\n", C_REG_BASE + regcount-2, C_REG_BASE + regcount-1);
                    regcount--;
                }
            }            
            break;
        case NODE_DIV:
            if(root->ptr1->nodetype==NODE_REG)
            {
                getreg(root->ptr1, reg1);
                if(root->ptr2->nodetype==NODE_REG)
                {
                    getreg(root->ptr2, reg2);
                    out_linecount+=2;
                    fprintf(fp, "MOV R%d, %s\nDIV R%d, %s\n", C_REG_BASE + regcount, reg1, C_REG_BASE + regcount, reg2);
                    regcount++;    
                }
                else if(root->ptr2->nodetype==NODE_NUM)
                {
                    out_linecount+=2;
                    fprintf(fp, "MOV R%d, %s\nDIV R%d, %d\n", C_REG_BASE + regcount, reg1, C_REG_BASE + regcount, root->ptr2->value);
                    regcount++;        
                }
                else
                {
                    out_linecount++;
                    fprintf(fp, "MOV R%d, %s\n", C_REG_BASE + regcount, reg1);
                    regcount++;
                    codegen(root->ptr2);
                    out_linecount++;
                    fprintf(fp, "DIV R%d, R%d\n", C_REG_BASE + regcount-2, C_REG_BASE + regcount-1);
                    regcount--;
                }        
                            
            }
            else
            {
                codegen(root->ptr1);            
                if(root->ptr2->nodetype==NODE_REG)
                {
                    getreg(root->ptr2, reg2);
                    out_linecount++;
                    fprintf(fp, "DIV R%d, %s\n", C_REG_BASE + regcount-1, reg2);    
                }
                else if(root->ptr2->nodetype==NODE_NUM)
                {
                    out_linecount++;
                    fprintf(fp, "DIV R%d, %d\n", C_REG_BASE + regcount-1, root->ptr2->value);    
                }
                else
                {
                    codegen(root->ptr2);
                    out_linecount++;
                    fprintf(fp, "DIV R%d, R%d\n", C_REG_BASE + regcount-2, C_REG_BASE + regcount-1);
                    regcount--;
                }
            }            
            break;
        case NODE_MOD:
            if(root->ptr1->nodetype==NODE_REG)
            {
                getreg(root->ptr1, reg1);
                if(root->ptr2->nodetype==NODE_REG)
                {
                    getreg(root->ptr2, reg2);
                    out_linecount+=2;
                    fprintf(fp, "MOV R%d, %s\nMOD R%d, %s\n", C_REG_BASE + regcount, reg1, C_REG_BASE + regcount, reg2);
                    regcount++;    
                }
                else if(root->ptr2->nodetype==NODE_NUM)
                {
                    out_linecount+=2;
                    fprintf(fp, "MOV R%d, %s\nMOD R%d, %d\n", C_REG_BASE + regcount, reg1, C_REG_BASE + regcount, root->ptr2->value);
                    regcount++;        
                }
                else
                {
                    out_linecount++;
                    fprintf(fp, "MOV R%d, %s\n", C_REG_BASE + regcount, reg1);
                    regcount++;
                    codegen(root->ptr2);
                    out_linecount++;
                    fprintf(fp, "MOD R%d, R%d\n", C_REG_BASE + regcount-2, C_REG_BASE + regcount-1);
                    regcount--;
                }                    
            }
            else
            {
                codegen(root->ptr1);            
                if(root->ptr2->nodetype==NODE_REG)
                {
                    getreg(root->ptr2, reg2);
                    out_linecount++;
                    fprintf(fp, "MOD R%d, %s\n", C_REG_BASE + regcount-1, reg2);    
                }
                else if(root->ptr2->nodetype==NODE_NUM)
                {
                    out_linecount++;
                    fprintf(fp, "MOD R%d, %d\n", C_REG_BASE + regcount-1, root->ptr2->value);    
                }
                else
                {
                    codegen(root->ptr2);
                    out_linecount++;
                    fprintf(fp, "MOD R%d, R%d\n", C_REG_BASE + regcount-2, C_REG_BASE + regcount-1);
                    regcount--;
                }
            }            
            break;
        case NODE_ASSIGN:        //assignment
            if(root->ptr1->nodetype==NODE_ADDR_EXPR) //[expr/no]=*
            {
                
                if(root->ptr1->ptr1->nodetype==NODE_NUM)    //[no]=*
                {
                    if(root->ptr2->nodetype==NODE_REG)        //[no]=reg
                    {
                        getreg(root->ptr2, reg2);
                        out_linecount++;
                        fprintf(fp, "MOV [%d], %s\n", root->ptr1->ptr1->value, reg2);    
                    }
                    else if(root->ptr2->nodetype==NODE_NUM)    //[no]=no
                    {
                        out_linecount++;
                        fprintf(fp, "MOV [%d], %d\n", root->ptr1->ptr1->value, root->ptr2->value);
                    }
                    else if(root->ptr2->nodetype==NODE_STRING)    //[no]=string
                    {
                        out_linecount++;
                        fprintf(fp, "MOV [%d], %s\n", root->ptr1->ptr1->value, root->ptr2->name);
                    }
                    else if(root->ptr2->nodetype==NODE_PORT)        //[no]=port
                    {
                        getreg(root->ptr2, reg2);
                        out_linecount++;
                        fprintf(fp, "PORT R%d, %s\nMOV [%d], R%d\n", C_REG_BASE + regcount, reg2, root->ptr1->ptr1->value, C_REG_BASE + regcount);    
                    }
                    else                    //[no]=expr
                    {
                        codegen(root->ptr2);
                        out_linecount++;
                        fprintf(fp, "MOV [%d], R%d\n", root->ptr1->ptr1->value, C_REG_BASE + regcount-1);
                        C_REG_BASE + regcount--;
                    }
                }
                else if(root->ptr1->ptr1->nodetype==NODE_REG)                //[reg]=*
                {
                    getreg(root->ptr1->ptr1, reg1);
                    if(root->ptr2->nodetype==NODE_REG)        //[reg]=reg
                    {
                        getreg(root->ptr2, reg2);
                        out_linecount++;
                        fprintf(fp, "MOV [%s], %s\n", reg1, reg2);    
                    }
                    else if(root->ptr2->nodetype==NODE_NUM)    //[reg]=no
                    {
                        out_linecount++;
                        fprintf(fp, "MOV [%s], %d\n", reg1, root->ptr2->value);
                    }
                    else if(root->ptr2->nodetype==NODE_STRING)    //[reg]=string
                    {
                        out_linecount++;
                        fprintf(fp, "MOV [%s], %s\n", reg1, root->ptr2->name);
                    }
                    else if(root->ptr2->nodetype==NODE_PORT)        //[reg]=port
                    {
                        getreg(root->ptr2, reg2);
                        out_linecount++;
                        fprintf(fp, "PORT R%d, %s\nMOV [%s], R%d\n", C_REG_BASE+ regcount, reg2, reg1, C_REG_BASE+regcount);    
                    }
                    else                    //[reg]=expr
                    {
                        codegen(root->ptr2);
                        out_linecount++;
                        fprintf(fp, "MOV [%s], R%d\n", reg1, C_REG_BASE + regcount-1);
                        regcount--;
                    }
                }
                else                //[expr]=*
                {
                    codegen(root->ptr1->ptr1);
                    if(root->ptr2->nodetype==NODE_REG)        //[expr]=reg
                    {
                        getreg(root->ptr2, reg2);
                        out_linecount++;
                        fprintf(fp, "MOV [R%d], %s\n", C_REG_BASE + regcount-1, reg2);    
                    }
                    else if(root->ptr2->nodetype==NODE_NUM)    //[expr]=no
                    {
                        out_linecount++;
                        fprintf(fp, "MOV [R%d], %d\n", C_REG_BASE + regcount-1, root->ptr2->value);
                    }
                    else if(root->ptr2->nodetype==NODE_STRING)    //[expr]=string
                    {
                        out_linecount++;
                        fprintf(fp, "MOV [R%d], %s\n", C_REG_BASE + regcount-1, root->ptr2->name);
                    }
                    else if(root->ptr2->nodetype==NODE_PORT)        //[expr]=port
                    {
                        getreg(root->ptr2, reg2);
                        out_linecount++;
                        fprintf(fp, "PORT R%d, %s\nMOV [R%d], R%d\n", C_REG_BASE + regcount, reg2, C_REG_BASE + regcount-1, C_REG_BASE + regcount);    
                    }
                    else                    //[expr]=expr
                    {
                        codegen(root->ptr2);
                        out_linecount++;
                        fprintf(fp, "MOV [R%d], R%d\n", C_REG_BASE + regcount-2, C_REG_BASE + regcount-1);
                        regcount--;
                    }
                    regcount--;
                }
            }
            else                //reg=*
            {            
                getreg(root->ptr1, reg1);
                if(root->ptr2->nodetype==NODE_REG)        //reg=reg
                {
                    getreg(root->ptr2, reg2);
                    out_linecount++;
                    fprintf(fp, "MOV %s, %s\n", reg1, reg2);    
                }
                else if(root->ptr2->nodetype==NODE_NUM)    //reg=no
                {
                    out_linecount++;
                    fprintf(fp, "MOV %s, %d\n", reg1, root->ptr2->value);
                }
                else if(root->ptr2->nodetype==NODE_STRING)    //reg=string
                {
                    out_linecount++;
                    fprintf(fp, "MOV %s, %s\n", reg1, root->ptr2->name);
                }
                else if(root->ptr2->nodetype==NODE_PORT)        //reg=port
                {
                    getreg(root->ptr2, reg2);
                    out_linecount++;
                    fprintf(fp, "PORT R%d, %s\nMOV %s, R%d\n", C_REG_BASE + regcount, reg2, reg1, C_REG_BASE + regcount);    
                }
                else                    //reg=expr
                {
                    codegen(root->ptr2);
                    out_linecount++;
                    fprintf(fp, "MOV %s, R%d\n", reg1, C_REG_BASE + regcount-1);
                    regcount--;
                }            
            }            
            break;
        case NODE_ADDR_EXPR:    //addresing
            codegen(root->ptr1);
            out_linecount++;
            fprintf(fp, "MOV R%d, [R%d]\n", C_REG_BASE + regcount-1, C_REG_BASE + regcount-1);
            break;
        case NODE_NUM:    //constants
            out_linecount++;
            fprintf(fp, "MOV R%d, %d\n", C_REG_BASE + regcount, root->value);
            regcount++;
            break;
        case NODE_STRING:    //string
            out_linecount++;
            fprintf(fp, "MOV R%d,  %s\n", C_REG_BASE + regcount, root->name);
            regcount++;
            break;
        case NODE_IF:    //IF statement ,  IF-ELSE statements
            push_label();            
            if(root->ptr1->nodetype==NODE_REG)
            {
                getreg(root->ptr1, reg1);
                fflush(fp);
                root_label->pos1 = ftell(fp);
                out_linecount++;
                fprintf(fp, "JZ %s, 00000\n", reg1);
                sprintf(root_label->instr1, "JZ %s,", reg1);
            }
            else
            {                
                codegen(root->ptr1);
                fflush(fp);
                root_label->pos1 = ftell(fp);
                out_linecount++;
                fprintf(fp, "JZ R%d, 00000\n", C_REG_BASE + regcount-1);
                sprintf(root_label->instr1, "JZ R%d,", C_REG_BASE + regcount-1);
                regcount--;
            }            
            codegen(root->ptr2);
            fflush(fp);
            root_label->pos2 = ftell(fp);
            out_linecount++;
            fprintf(fp, "JMP 00000\n");
            sprintf(root_label->instr2, "JMP");
            fflush(fp);
            temp_pos = ftell(fp);
            fseek(fp,root_label->pos1,SEEK_SET);
            fprintf(fp,"%s %05d",root_label->instr1,addrBaseVal +  out_linecount*2);
            fseek(fp,temp_pos,SEEK_SET);
            codegen(root->ptr3);
            fflush(fp);
            temp_pos = ftell(fp);
            fseek(fp,root_label->pos2,SEEK_SET);
            fprintf(fp,"%s %05d",root_label->instr2,addrBaseVal +  out_linecount*2);
            fseek(fp,temp_pos,SEEK_SET);
            pop_label();
            break;
        case NODE_WHILE:    //WHILE loop
            push_label();
            push_while(root_label->i);
            root_label->pos1=addrBaseVal +  out_linecount*2;
            root_while->pos1=addrBaseVal +  out_linecount*2;
            if(root->ptr1->nodetype==NODE_REG)
            {
                getreg(root->ptr1, reg1);
                fflush(fp);
                root_label->pos2 = ftell(fp);
                out_linecount++;
                fprintf(fp, "JZ %s, 00000\n", reg1);
                sprintf(root_label->instr2, "JZ %s,", reg1);
            }
            else
            {                
                codegen(root->ptr1);
                fflush(fp);
                root_label->pos2 = ftell(fp);
                out_linecount++;
                fprintf(fp, "JZ R%d, 00000\n", C_REG_BASE + regcount-1);
                sprintf(root_label->instr2, "JZ R%d,", C_REG_BASE + regcount-1);
                regcount--;
            }            
            codegen(root->ptr2);
            out_linecount++;
            fprintf(fp, "JMP %ld\n", root_label->pos1);
            fflush(fp);
            temp_pos = ftell(fp);
            fseek(fp,root_label->pos2,SEEK_SET);
            fprintf(fp,"%s %05d",root_label->instr2,addrBaseVal +  out_linecount*2);
            fseek(fp,temp_pos,SEEK_SET);
            use_jmp_points(root_while->points);
            pop_while();
            pop_label();
            break;
        case NODE_BREAK:    //BREAK loop
            add_jmp_point("JMP");
            out_linecount++;
            fprintf(fp, "JMP 00000\n");
            break;
        case NODE_CONTINUE:    //CONTINUE loop
            out_linecount++;
            fprintf(fp, "JMP %ld\n", root_while->pos1);
            break;
        case NODE_LOADI:    //Loadi
            if(root->ptr1->nodetype==NODE_REG)
            {
                getreg(root->ptr1, reg1);
                if(root->ptr2->nodetype==NODE_REG)
                {
                    getreg(root->ptr2, reg2);
                    out_linecount++;
                    fprintf(fp, "LOADI %s, %s\n", reg1, reg2);                        
                }
                else if(root->ptr2->nodetype==NODE_NUM)
                {
                    out_linecount++;
                    fprintf(fp, "LOADI %s, %d\n", reg1, root->ptr2->value);
                }
                else
                {
                    codegen(root->ptr2);
                    out_linecount++;
                    fprintf(fp, "LOADI %s, R%d\n", reg1, C_REG_BASE + regcount-1);
                    regcount--;
                }                    
            }
            else
            {
                codegen(root->ptr1);            
                if(root->ptr2->nodetype==NODE_REG)
                {
                    getreg(root->ptr2, reg2);
                    out_linecount++; fprintf(fp, "LOADI R%d, %s\n", C_REG_BASE + regcount-1, reg2);    
                }
                else if(root->ptr2->nodetype==NODE_NUM)
                {
                    out_linecount++;
                    fprintf(fp, "LOADI R%d, %d\n", C_REG_BASE + regcount-1, root->ptr2->value);
                }
                else
                {
                    codegen(root->ptr2);
                    out_linecount++; fprintf(fp, "LOADI R%d, R%d\n", C_REG_BASE + regcount-2, C_REG_BASE + regcount-1);
                    regcount--;
                }
                regcount--;
            }            
            break;
        /*case 'S':    //Store or store immediate(currently this feature is removed)
            if(root->ptr1->nodetype==NODE_REG)
            {
                getreg(root->ptr1, reg1);
                if(root->ptr2->nodetype==NODE_REG)
                {
                    getreg(root->ptr2, reg2);
                    out_linecount++;
                    fprintf(fp, "STOREI %s, %s\n", reg2, reg1);                        
                }
                else if(root->ptr2->nodetype==NODE_NUM)
                {
                    out_linecount++;
                    fprintf(fp, "STOREI %d, %s\n", root->ptr2->value, reg1 );
                }
                else
                {
                    codegen(root->ptr2);
                    out_linecount++; fprintf(fp, "STOREI R%d, %s\n", C_REG_BASE + regcount-1, reg1);
                    regcount--;
                }                    
            }
            else
            {
                codegen(root->ptr1);            
                if(root->ptr2->nodetype==NODE_REG)
                {
                    getreg(root->ptr2, reg2);
                    out_linecount++; fprintf(fp, "STOREI %s, R%d\n", reg2, C_REG_BASE + regcount-1);    
                }
                else if(root->ptr2->nodetype==NODE_NUM)
                {
                    out_linecount++;
                    fprintf(fp, "STOREI %d, R%d\n", root->ptr2->value, C_REG_BASE + regcount-1);
                }
                else
                {
                    codegen(root->ptr2);
                    out_linecount++;
                    fprintf(fp, "STOREI R%d, R%d\n", C_REG_BASE + regcount-1, C_REG_BASE + regcount-2);
                    regcount--;
                }
                regcount--;
            }            
            break;   */        
        case NODE_LOAD:    //load or Load asynchronous(which is the default load)
            if(root->ptr1->nodetype==NODE_REG)
            {
                getreg(root->ptr1, reg1);
                if(root->ptr2->nodetype==NODE_REG)
                {
                    getreg(root->ptr2, reg2);
                    out_linecount++;
                    fprintf(fp, "LOAD %s, %s\n", reg1, reg2);                        
                }
                else if(root->ptr2->nodetype==NODE_NUM)
                {
                    out_linecount++;
                    fprintf(fp, "LOAD %s, %d\n", reg1, root->ptr2->value);
                }
                else
                {
                    codegen(root->ptr2);
                    out_linecount++;
                    fprintf(fp, "LOAD %s, R%d\n", reg1, C_REG_BASE + regcount-1);
                    regcount--;
                }                    
            }
            else
            {
                codegen(root->ptr1);            
                if(root->ptr2->nodetype==NODE_REG)
                {
                    getreg(root->ptr2, reg2);
                    out_linecount++; fprintf(fp, "LOAD R%d, %s\n", C_REG_BASE + regcount-1, reg2);    
                }
                else if(root->ptr2->nodetype==NODE_NUM)
                {
                    out_linecount++;
                    fprintf(fp, "LOAD R%d, %d\n", C_REG_BASE + regcount-1, root->ptr2->value);
                }
                else
                {
                    codegen(root->ptr2);
                    out_linecount++; fprintf(fp, "LOAD R%d, R%d\n", C_REG_BASE + regcount-2, C_REG_BASE + regcount-1);
                    regcount--;
                }
                regcount--;
            }            
            break;
        case NODE_STORE:    //store or Store asnchronous (which is the default store)
            if(root->ptr1->nodetype==NODE_REG)
            {
                getreg(root->ptr1, reg1);
                if(root->ptr2->nodetype==NODE_REG)
                {
                    getreg(root->ptr2, reg2);
                    out_linecount++;
                    fprintf(fp, "STORE %s, %s\n", reg2, reg1);                        
                }
                else if(root->ptr2->nodetype==NODE_NUM)
                {
                    out_linecount++;
                    fprintf(fp, "STORE %d, %s\n", root->ptr2->value, reg1 );
                }
                else
                {
                    codegen(root->ptr2);
                    out_linecount++;
                    fprintf(fp, "STORE R%d, %s\n", C_REG_BASE + regcount-1, reg1);
                    regcount--;
                }                    
            }
            else
            {
                codegen(root->ptr1);            
                if(root->ptr2->nodetype==NODE_REG)
                {
                    getreg(root->ptr2, reg2);
                    out_linecount++;
                    fprintf(fp, "STORE %s, R%d\n", reg2, C_REG_BASE + regcount-1);    
                }
                else if(root->ptr2->nodetype==NODE_NUM)
                {
                    out_linecount++;
                    fprintf(fp, "STORE %d, R%d\n", root->ptr2->value, C_REG_BASE + regcount-1);
                }
                else
                {
                    codegen(root->ptr2);
                    out_linecount++;
                    fprintf(fp, "STORE R%d, R%d\n", C_REG_BASE + regcount-1, C_REG_BASE + regcount-2);
                    regcount--;
                }
                regcount--;
            }            
            break;
        case NODE_BACKUP:
            out_linecount++;
            fprintf(fp, "BACKUP\n");
            break;
        case NODE_RESTORE:
            out_linecount++;
            fprintf(fp, "RESTORE\n");
            break;
        case NODE_IRETURN:    //Ireturn
            out_linecount++;
            fprintf(fp, "IRET\n");
            break;
        case NODE_REG:    //register
             getreg(root, reg1);
             out_linecount++;
             fprintf(fp, "MOV R%d, %s\n", C_REG_BASE + regcount, reg1);
             regcount++;
             break;
        case NODE_HALT:    //halt
            out_linecount++;
            fprintf(fp, "HALT\n");
            break;
        case NODE_BREAKPOINT:    //checkpoint
            out_linecount++;
            fprintf(fp, "BRKP\n");
            break;
        case NODE_READ:
            out_linecount++;
            fprintf(fp, "IN\n");
            break;
        case NODE_READI:    //readi
            getreg(root->ptr1, reg1);
            out_linecount+=2;
            fprintf(fp, "INI\nPORT %s, P0\n", reg1);
            break;
        case NODE_PRINT:    //print
            codegen(root->ptr1);
            out_linecount+=2;
            fprintf(fp, "PORT P0, R%d\nOUT\n", C_REG_BASE + regcount-1);
            regcount--;
            break;
        case NODE_INLINE:    //INLINE
            out_linecount++;
            fprintf(fp, "%s\n",root->ptr1->name);
            break;
        case NODE_ENCRYPT:    //encrypt
            getreg(root->ptr1, reg1);
            out_linecount++;
            fprintf(fp, "ENCRYPT %s\n", reg1);
            break;
        default:
            printf("Unknown Command %d %s\n", root->nodetype, root->name);        //Debugging
            return;
    }
}

void expandpath(char *path) // To expand environment variables in path
{
    char *rem_path = strdup(path);
    char *token = strsep(&rem_path, "/");
    if(rem_path!=NULL)
        sprintf(path,"%s/%s",getenv(++token)!=NULL?getenv(token):token-1,rem_path);
    else
        sprintf(path,"%s",getenv(++token)!=NULL?getenv(token):token-1);
}

void remfilename(char *pathname)
{
    int l = strlen(pathname);
    int i = l-1;    
    while(pathname[i] != '/' && i>=0)
    {
        i--;
    }
    pathname[i+1]='\0';    
}
