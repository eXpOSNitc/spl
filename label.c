#include "label.h"

static label *_root_label=NULL;
static int _namegen=1;
static l_while *_rootWhile=NULL;
/*Create a new label with a random unused name*/
label* create_label()
{
    char name[30];
    sprintf(name,"_L%d",_namegen);
    _namegen++;
    return add_label(name);
}

label* add_label(const char* name)
{
    label* new_label=NULL;
    new_label=(label*)malloc(sizeof(label));
    new_label->name=strdup(name);
    new_label->next=_root_label;
    _root_label=new_label;
    return new_label;
}

/*Get the label element with the name specified, if it exists, else return NULL*/
label* get_label(const char* name)
{
    label *temp=_root_label;
    while(temp!=NULL)
    {
        if(strcmp(temp->name, name)==0)
            return temp;
        temp=temp->next;
    }
    return NULL;
}

void label_pushWhile(label *start, label *end)
{
    l_while *newl=(l_while*)malloc(sizeof(l_while));
    newl->start=start;
    newl->end=end;
    newl->next=_rootWhile;
    _rootWhile=newl;
}

void label_popWhile()
{
    l_while *temp=_rootWhile;
    _rootWhile=_rootWhile->next;
    free(temp);
}

label* label_getWhileEnd()
{
    return _rootWhile->end;   
}

label* label_getWhileStart()
{
    return _rootWhile->start;   
}

char* label_getName(label *ll)
{
    return ll->name;
}
