
#ifndef LABEL_H
#define LABEL_H
typedef struct labelt
{
    char *name;
    struct labelt *next;
}label;

label* create_label();
label* add_label(const char* name);
#endif
