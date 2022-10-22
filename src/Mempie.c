#include "Mempie.h"


struct Mempie_pile{
    int *check;
    size_t size;
    Mempie_pile *prev, *next;
};

void* (*salloc)(size_t) = &malloc; 

void Mempie_changealloc(void* (*alloc)(size_t)){
    salloc = alloc;
}

static int Mempie_check;

void* Mempie_alloc(Mempie_pile* last ,size_t size){
    if(last) last--;
    printf("Mempie allocation!!\n");
    Mempie_pile *alloc = (Mempie_pile *)salloc(sizeof(Mempie_pile)+size);
    alloc->check = &Mempie_check;
    alloc->size = size;
    char * s = (char*)(alloc + 1);
    alloc->next = NULL;
    alloc->prev = NULL;
    for (char* c = s; c < s + size; c++) *c = 0;
    alloc->prev = last;
    if (last) {
        if (last->next)
            alloc->next = last->next;
        last->next = alloc;
        
    }
    return alloc+1;
}

int Mempie_ispile(char* block){
    Mempie_pile *alloc =(Mempie_pile *) block; alloc--;
    return (*((int **)alloc) == &Mempie_check);
}

size_t Mempie_size(char* block){
    Mempie_pile *alloc = (Mempie_pile *)block; alloc--;
    return alloc->size;
}

void Mempie_free(char* block){
    Mempie_pile *alloc = (Mempie_pile *)block; alloc--;
    if (alloc->next)
        alloc->next->prev = alloc->prev;
    if (alloc->prev)
        alloc->prev->next = alloc->next;
    free(alloc);
}

void Mempie_showAll(char* block){
    Mempie_pile *alloc =(Mempie_pile *) block; alloc--;
    Mempie_pile *k = 0;
    for (Mempie_pile *l = alloc->prev; l != NULL; ){
        printf("to the left %d\n", l);
        l = l->prev;
    }
    for (Mempie_pile *r = alloc->next; r != NULL; ){
        printf("to the right %d\n", r);
        r = r->next;
    }
}

void Mempie_freeall(char* block){
    Mempie_pile *alloc =(Mempie_pile *) block; alloc--;
    Mempie_pile *k = 0;
    for (Mempie_pile *l = alloc->prev; l != NULL; ){
        if (l){ k = l->prev; free(l); l = k;}
        printf("check\n");
    }
    for (Mempie_pile *r = alloc->next; r != NULL; ){
        if (r){ k = r->next; free(r); r = k;}
        printf("check\n");
    }
    free(alloc);
    printf("check\n");
}

