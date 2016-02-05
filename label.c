#include "label.h"

static label *_root_label=NULL;
static int _namegen=1;

/*Create a new label with a random unused name*/
int create_label()
{
    char name[30];
    label* new_label=NULL:
    sprintf(name,"_LL%d",_namegen);
    _namegen++;
    return add_label(name);
}

label* add_label(const char* name)
{
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
