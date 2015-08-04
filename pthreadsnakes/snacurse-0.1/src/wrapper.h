#ifndef WRAPPER_H
#define WRAPPER_H

#include <stdlib.h>
#include <ncurses.h>
#include <semaphore.h>
#include <pthread.h>

void Start_color();
void *Malloc(size_t size);
void Sem_init(sem_t *s, int pshared, unsigned int value);
void Sem_wait(sem_t *s);
void Sem_post(sem_t *s);
void Pthread_create(pthread_t *tidp, pthread_attr_t *attrp, void * (*routine)(void *), void *argp);
void Pthread_join(pthread_t tid, void **thread_return);
void Pthread_detach(pthread_t tid);
pthread_t Pthread_self(void);


#endif
