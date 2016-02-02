#include "node.h"
#include <stdlib.h>
#include <string.h>

struct tree* create_tnode(char nodetype, char *name, int value)
{
    struct tree* tnode=malloc(sizeof(struct tree));
    tnode->nodetype=nodetype;
    if(name!=NULL)
        tnode->name=strdup(name);
    tnode->value=value;
    tnode->entry=NULL;
    tnode->ptr1=NULL;
    tnode->ptr2=NULL;
    tnode->ptr3=NULL;
    return tnode;
}
