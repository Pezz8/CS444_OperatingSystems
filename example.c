#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#define NUMBER OF THREADS 10
void *pr int hello world(void *tid)
{
/* This function prints the thread’s identifier and then exits. */
pr intf("Hello World. Greetings from thread %d\n", tid);
pthread exit(NULL);
}
int main(int argc, char *argv[])
{
/* The main program creates 10 threads and then exits. */
pthread t threads[NUMBER OF THREADS];
int status, i;
for(i=0; i < NUMBER OF THREADS; i++) {
pr intf("Main here. Creating thread %d\n", i);
status = pthread create(&threads[i], NULL, print hello world, (void *)i);
if (status != 0) {
pr intf("Oops. pthread create returned error code %d\n", status);
exit(-1);
}
}
exit(NULL);
}