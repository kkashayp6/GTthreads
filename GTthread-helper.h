#ifndef LIBRARY
#define LIBRARY


#include "malloc.h"
#include "sys/types.h"
#include "sys/time.h"
#include "ucontext.h"
#include <signal.h>
#include <stdlib.h>
#include <stdio.h>


typedef int bool;
#define true 1
#define false 0


typedef struct queue_node_t{
ucontext_t node_context;
struct queue_node_t* next;
struct queue_node_t* prev;
int node_id;
int parent_id;
void* return_value;
bool thread_complete;
bool thread_cancelled;
bool thread_exited;
}queue_node;

typedef struct {
queue_node* front;
queue_node* back;
}queue_t;

typedef struct r_queue_t_node{
long thread_id;
void* return_value;
struct r_queue_t_node* next;
struct r_queue_t_node* prev;
bool thread_complete;
bool thread_cancelled;
bool thread_exited;
bool thread_read;
bool  thread_written;
}r_queue_node;

typedef struct {
r_queue_node* front;
r_queue_node* back;
}r_queue;

typedef int gtthread_t;
typedef int gtthread_mutex_t;

typedef struct mutex_t
{
gtthread_mutex_t id;
bool locked;
}mutex;


/*define the variables*/
long time_period;
#define MEM 64000
#define MAX 10000
#define GTTHREAD_CANCELED ((void *)(size_t) -1)
queue_t queue;
r_queue return_queue;
queue_node* current;
struct sigaction act, oact;
struct itimerval it;
sigset_t x;
queue_node* current;
gtthread_t main1;
int thread_id;
int mutexindex;
mutex mutexArray[MAX];

/*define the functions*/
extern void block();
extern void unblock();
extern void timer();
extern void killing();
extern void Scheduler();
extern void return_queue_manipulation(int);
extern void return_queue_value(int);
extern void add_node(queue_node*);
extern void thread_start(void *(*start_routine)(void *),void *arg);
extern void eventhandler(int sig, siginfo_t* siginfo, void* rcontext);
extern int get_context(queue_node*);
extern void return_queue_cancel(int);

#endif


