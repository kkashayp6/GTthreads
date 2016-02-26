#include "GTthread.h"
#include "GTthread-helper.h"

/*queue functions*/

void add_node(queue_node* thread)
{
  if(queue.front==NULL)
  {
    queue.front=thread;
    queue.back=thread;
    thread->next=thread;
    thread->prev=thread;
  }
  else if (queue.back==queue.front)
  {
    queue.back=thread;
    queue.front->next=thread;
    queue.back->prev=queue.front;
    queue.back->next=queue.front;
    queue.front->prev=queue.back;
  }
  else
  {
    queue.back->next=thread;
    thread->prev=queue.back;
    queue.back=thread;
    queue.back->next=queue.front;
    queue.front->prev=queue.back;
  }
}

void return_queue_manipulation(int id)
{
  r_queue_node* new=(r_queue_node*)malloc(sizeof(r_queue_node));
  new->thread_id=id;
  if(return_queue.front==NULL)
  {
    return_queue.front=new;
    return_queue.back=new;
    new->prev=NULL;
    new->next=NULL;
  }
  else if(return_queue.front==return_queue.back)
  {
    return_queue.back=new;
    return_queue.front->next=new;
    return_queue.back->prev=return_queue.front;
    return_queue.back->next=NULL;
  }
  else
  {
    return_queue.back->next=new;
    new->prev=return_queue.back;
    return_queue.back=new;
    return_queue.back->next=NULL;
  }
}

void return_queue_value(int id)
{
  r_queue_node* p=(r_queue_node*)malloc(sizeof(r_queue_node));
  bool flag=false;
  p=return_queue.front;
  for(p=return_queue.front;p<=return_queue.back;p=p->next)
  {
    if(p->thread_id==id)
    {
      if(current->thread_cancelled==true)
      {
        p->thread_cancelled=true;
        return;
      }
      if(current->thread_exited==true) p->thread_exited=true;
      p->thread_complete=true;
      p->return_value=current->return_value;
      flag=true;
    }
    if(p==return_queue.back) flag=true;
    if(flag==true) break;
  }
}

void return_queue_cancel(int id)
{
  r_queue_node* p=(r_queue_node*)malloc(sizeof(r_queue_node));  
  bool flag=false;
  p=return_queue.front;
  for(p=return_queue.front;p<=return_queue.back;p=p->next)
  {
    if(p->thread_id==id)
    {
      flag=true;
      if(p->thread_exited!=true && p->thread_complete!=true)
      {
        p->thread_cancelled=true;
        p->thread_complete=true;
      }
    }
    if(p==return_queue.back) flag=true;
    if(flag==true) break;
  }
}


int get_context(queue_node* thread)
{
  getcontext(&(thread->node_context));
  thread->node_context.uc_link=NULL;
  thread->node_context.uc_stack.ss_sp=malloc(MEM);
  thread->node_context.uc_stack.ss_size=MEM;
  thread->node_context.uc_stack.ss_flags=0;
  if(thread->node_context.uc_stack.ss_sp==0) return 1;
  return 0;
}

void thread_start(void *(*start_routine)(void *),void *arg)
{
  current->return_value=start_routine(arg);
  killing();
}

/*Scheduler functions*/

void eventhandler(int sig, siginfo_t* siginfo, void* rcontext)
{
  current->node_context=*((ucontext_t*)rcontext);
  Scheduler();
}

void Scheduler()
{
  if(queue.front==queue.back)
  {
    timer();
    setcontext(&(current->node_context));
  }
  else
  {
    current=current->next;
    timer();
    setcontext(&(current->node_context));
  }
}

void killing()
{
  block();
  return_queue_value(current->node_id);
  if(queue.front!=NULL)
  {
    if(queue.front==queue.back)
    {
      queue.front=NULL;
      queue.back=NULL;
      unblock();
    }
    else
    {
      current->next->prev=current->prev;
      current->prev->next=current->next;
      if(current==queue.front) queue.front=current->next;
      if(current==queue.back) queue.back=current->prev;
      free(current);
      current=current->next;
      unblock();
      timer();
      setcontext(&(current->node_context));
    }
  }
}

/*timer functions*/

void timer()
{
  act.sa_sigaction = eventhandler;
  sigemptyset(&act.sa_mask);
  act.sa_flags = SA_SIGINFO;
  sigaction(SIGVTALRM, &act, &oact);
  it.it_interval.tv_sec = 0;
  it.it_interval.tv_usec = time_period;
  it.it_value.tv_sec = 0;
  it.it_value.tv_usec = time_period;
  setitimer(ITIMER_VIRTUAL, &it, NULL);
}

void block()
{
  sigemptyset(&x);
  sigaddset(&x,SIGVTALRM);
  sigprocmask(SIG_BLOCK, &x, NULL);
}

void unblock()
{
  sigemptyset(&x);
  sigaddset(&x,SIGVTALRM);
  sigprocmask(SIG_UNBLOCK, &x, NULL);
}

/*mutex functions*/

int gtthread_mutex_init(gtthread_mutex_t* mutex)
{
  mutexindex++;
  *mutex = mutexindex;
  mutexArray[mutexindex].locked = false;
  return 0;
}

int gtthread_mutex_lock(gtthread_mutex_t* mutex)
{
  while(mutexArray[*mutex].locked==true)
  {
    gtthread_yield();
  }
  mutexArray[*mutex].locked=true;
  mutexArray[*mutex].id=current->node_id;
  return 0;
}

int gtthread_mutex_unlock(gtthread_mutex_t* mutex)
{
  if(mutexArray[*mutex].locked==true && mutexArray[*mutex].id==current->node_id)
  {
    mutexArray[*mutex].locked=false;
  }
  return 0;
}


