#include "wrapper.h"

void error_exit(char *str){

     endwin();
     fprintf(stderr,str);
     exit(1);
}

void *Malloc(size_t size){
     void *temp;

     if((temp = malloc(size)) == NULL)
	  error_exit("Error: malloc failed\n");
     return temp;
}

/************************************************
 * Wrappers for Pthreads thread control functions
 ************************************************/

void Pthread_create(pthread_t *tidp, pthread_attr_t *attrp, 
		    void * (*routine)(void *), void *argp) 
{
     int rc;

     if ((rc = pthread_create(tidp, attrp, routine, argp)) != 0)
	  error_exit("Error: pthread_create failed\n");
}

void Pthread_cancel(pthread_t tid) {
     int rc;

     if ((rc = pthread_cancel(tid)) != 0)
	  error_exit("Error: pthread_cancel failed\n");
}

void Pthread_join(pthread_t tid, void **thread_return) {
     int rc;

     if ((rc = pthread_join(tid, thread_return)) != 0)
	  error_exit("Error: pthread_join failed\n");
}

/* $begin detach */
void Pthread_detach(pthread_t tid) {
     int rc;

     if ((rc = pthread_detach(tid)) != 0)
	  error_exit("Error: pthread_detach failed\n");
}
/* $end detach */

void Pthread_exit(void *retval) {
     pthread_exit(retval);
}

pthread_t Pthread_self(void) {
     return pthread_self();
}
 
void Pthread_once(pthread_once_t *once_control, void (*init_function)()) {
     pthread_once(once_control, init_function);
}

void Start_color(){
     if(has_colors() == FALSE)
	  error_exit("Error: your terminal does not support colors\n");
     start_color();
}

void Sem_init(sem_t *s,int pshared, unsigned int value){
     if(sem_init(s, pshared, value) < 0)
	  error_exit("Error: sem_init failed\n");
}
void Sem_wait(sem_t *s){
     if(sem_wait(s) < 0)
	  error_exit("Error: sem_wait failed\n");
}

void Sem_post(sem_t *s){
     if(sem_post(s) < 0)
	  error_exit("Error: sem_post failed\n");
}
