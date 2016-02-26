#include "GTthread.h"
#include "GTthread-helper.h"

void gtthread_init(long period)
{
  time_period=period;
  thread_id=0;
  mutexindex=0;
  queue_node* thread;
  thread=(queue_node*)malloc(sizeof(queue_node));
  thread_id++;
  main1=thread_id;
  thread->node_id=main1;
  thread->thread_cancelled=false;
  thread->thread_complete=false;
  add_node(thread);
  get_context(thread);
  current=queue.front;
  return_queue_manipulation(thread->node_id);
  timer();
}

int gtthread_create(gtthread_t* t,void *(*start_routine)(void*),void* arg)
{
  thread_id++;
  *t=thread_id;
  queue_node* thread;
  thread=(queue_node*)malloc(sizeof(queue_node));
  thread->node_id=*t;
  thread->thread_cancelled=false;
  thread->thread_complete=false;
  add_node(thread);
  return_queue_manipulation(thread->node_id);
  int result;
  result=get_context(thread);
  if(result!=0) return 1;
  makecontext(&(thread->node_context),thread_start,2,start_routine,arg);
  return 0;
}

int gtthread_join(gtthread_t thread,void** ret_val)
{
  r_queue_node* p;
  p=(r_queue_node*)malloc(sizeof(r_queue_node));
  bool work_done;
  work_done=false;
  for(p=return_queue.front;p<=return_queue.back;p=p->next)
  {
    if(p->thread_id==thread)
    {
      work_done=true;
      while(p->thread_complete!=true);
      if(p->thread_cancelled==true)
      {
        if(ret_val==NULL) return 0;
        *ret_val=GTTHREAD_CANCELED;
        return 0;
      }
      if(ret_val==NULL) return 0;
      else *ret_val=p->return_value;
    }
    if(p==return_queue.back) work_done=true;
    if(work_done==true) break;
  }
  return 0;
}

int gtthread_yield(void)
{
  if(queue.front==queue.back) return 1;
  else
  {
    raise(SIGVTALRM);
    return 1;
  }
}

int gtthread_equal(gtthread_t t1,gtthread_t t2)
{
  if(t1==t2) return 1;
  else return 0;
}

gtthread_t gtthread_self()
{
  return current->node_id;
}

void gtthread_exit(void* retval)
{
  current->thread_exited=true;
  current->return_value=(void*)retval;
  killing();
}

int gtthread_cancel(gtthread_t thread)
{
  queue_node* p;
  p=(queue_node*)malloc(sizeof(queue_node));
  p=queue.front;
  bool flag; 
  bool found;
  flag=false;
  found=false;
  block();
  return_queue_cancel(thread);
  while(flag==false)
  {
    if(p->node_id==thread)
    {
      flag=true;
      found=true;
      if(queue.front==queue.back)
      {
        exit(1);
      }
      if(p==current)
      {
        current->next->prev=current->prev;
        current->prev->next=current->next;
        if(current==queue.front) queue.front=current->next;
        if(current==queue.back) queue.back=current->prev;
        current=current->next;
        free(p);
        unblock();
        timer();
        setcontext((&current->node_context));
      }
      else
      {
        p->next->prev=p->prev;
        p->prev->next=p->next;
        if(p==queue.front) queue.front=p->next;
        if(p==queue.back) queue.back=p->prev;
        free(p);
      }
    }
    if(p==queue.back) flag=true;
    if(flag==true) break;
    p=p->next;
  }
  unblock();
  timer();
  if(found==false) return 1;
  return 0;
}

