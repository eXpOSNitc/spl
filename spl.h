#include<string.h>
#include "data.h"
#include "node.h"
#define LEGAL 0

#define OS_STARTUP      512
#define EX_HANDLER      1024
#define T_INTERRUPT     2048
#define D_INTERRUPT     3072
#define C_INTERRUPT     4096
#define INTERRUPT       2048
//Default values of Pre-defined constants in SPL(These values if and only if the values are not specified in the "constants" file)
//TODO defualt values to be obtained from os team
#define INODE_TABLE             512
#define DISK_FREE_LIST          512
#define ROOT_FILE               512
#define USER_TABLE              512
#define PROCESS_TABLE           512
#define FILE_TABLE              512
#define SEMAPHORE_TABLE         512
#define DISK_STATUS_TABLE       512
#define BUFFER_TABLE            512
#define SYSTEM_STATUS_TABLE     512
#define TERMINAL_STATUS_TABLE   512
#define MEMORY_FREE_LIST        512
#define EXCEPTION               512
#define TIMER                   512
#define DISK                    512
#define CONSOLE                 512
#define INT_4                   512
#define INT_5                   512
#define INT_6                   512
#define INT_7                   512
#define INT_8                   512
#define INT_9                   512
#define INT_10                  512
#define INT_11                  512
#define INT_12                  512
#define INT_13                  512
#define INT_14                  512
#define INT_15                  512
#define INT_16                  512
#define INT_17                  512
#define INT_18                  512
#define MOD_0                   512
#define MOD_1                   512
#define MOD_2                   512
#define MOD_3                   512
#define MOD_4                   512
#define MOD_5                   512
#define MOD_6                   512
#define MOD_7                   512
#define DISK_SWAP_AREA          512

#define PAGE_PER_INTERRUPT      2
#define PAGE_SIZE               512


extern int linecount;
extern unsigned long temp_pos; //temporary lseek
extern int out_linecount; //no of lines of code generated
extern int addrBaseVal;    //Starting Address where the compiled code will be loaded
extern int flag_break;
extern int regcount;


extern FILE *fp;
                        //start labels
extern int labelcount;
struct jmp_point
{
    unsigned long pos;
    char instr[32];
    struct jmp_point *next;
    struct jmp_point *points; 
};
struct label
{
    int i;
    unsigned long pos1,pos2;
    char instr1[32],instr2[32];
    struct label *next;
    struct jmp_point *points;  
};
extern struct label *root_label, *root_while;

void push_label();
int pop_label();
void push_while(int n);
void pop_while();
void add_jmp_point(char instr[32]);
void use_jmp_points(struct jmp_point *root);
                        ///end labels
                        
                        ///starting named labels (for goto and call)
struct named_label
{
    char name[30];//TODO
    
};
                        ///end named labels
                        ///start constants and aliasing

extern struct define *root_define;
extern char alias_table[8][30];
struct define* lookup_constant(char *name);
extern int depth;

struct alias
{
    char name[30];
    int no, depth;
    struct alias *next;
};
extern struct alias *root_alias;

struct alias * lookup_alias(char *name);
struct alias * lookup_alias_reg(int no);
void push_alias(char *name, int no);
void pop_alias();
void insert_constant(char *name, int value);
void add_predefined_constants();

struct tree * substitute_id(struct tree *id);
                            ///end of constants and alias
                            ///start tree create fns
struct tree * create_nonterm_node(char *name, struct tree *a, struct tree *b);
struct tree * create_tree(struct tree *a, struct tree *b, struct tree *c, struct tree *d);
                            ///end tree create fns
void getreg(struct tree *root, char reg[]);

void codegen(struct tree * root);

void expandpath(char *path); // To expand environment variables in path

void remfilename(char *pathname);
