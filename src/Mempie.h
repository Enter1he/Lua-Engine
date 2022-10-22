// Mempie.h
#ifndef MEMPIE_HEADER
#define MEMPIE_HEADER
#include <stdio.h>
#include <stdlib.h>


typedef struct Mempie_pile Mempie_pile;

void* Mempie_alloc(Mempie_pile* last ,size_t size);

void Mempie_changealloc(void* (*alloc)(size_t));

int Mempie_ispile(char* block);

size_t Mempie_size(char* block);

void Mempie_free(char* block);

void Mempie_freeall(char* block);

#endif