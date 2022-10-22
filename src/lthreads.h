#ifndef LTHREADS_H
#define LTHREADS_H

#include <pthread.h>


#define T_args void* args

typedef void* TCallback;


typedef struct _pthread {
  pthread_t thread; 
  pthread_cond_t on; 
  pthread_mutex_t mute;
}Pthread;

void CreatePthread(Pthread *t, TCallback tfunc){
    
  pthread_create(&t->thread, NULL, tfunc, NULL);
  pthread_cond_init(&t->on, NULL);
  pthread_mutex_init(&t->mute, NULL);
}

void DestroyPthread(Pthread *t){
    
  pthread_detach(t->thread);
  pthread_cond_destroy(&t->on);
  pthread_mutex_destroy(&t->mute);
}


#endif