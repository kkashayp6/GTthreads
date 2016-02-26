#include "GTthread.h"
#include "GTthread-helper.h"

#define SIZE 5
int philosophers[SIZE];
int forks[SIZE+1];
  
void thread_function()
{
  int id;
  id=gtthread_self()-1;
  int leftfork,rightfork;
  leftfork=id+1;
  rightfork=id;
  if(id+1>SIZE) leftfork=1;
  int i=0;
  while(1)
  {
    if(id==2)
  {
    gtthread_mutex_lock(&forks[rightfork]);
    for(i=0;i<1000000000;i++);
    gtthread_mutex_lock(&forks[leftfork]);
    printf("philosopher %d starts eating\n",id);
  }
  else
  {
    for(i=0;i<1000000000;i++);
    gtthread_mutex_lock(&forks[leftfork]);
    for(i=0;i<1000000000;i++);
    gtthread_mutex_lock(&forks[rightfork]);
    printf("philosopher %d starts eating\n",id);
  }
  for(i=0;i<100000000;i++);
  gtthread_mutex_unlock(&forks[leftfork]);
  for(i=0;i<1000000000;i++);
  gtthread_mutex_unlock(&forks[rightfork]);
  for(i=0;i<1000000000;i++);
  printf("philosopher %d done eating\n",id);
  for(i=0;i<1000000000;i++);
  }
}

int main()
{
  gtthread_init(50000L);
  int i=0;
  for(i=1;i<=SIZE;i++)
  {
    gtthread_mutex_init(&forks[i]);
  }
  for(i=0;i<SIZE;i++)
  {
    gtthread_create(&philosophers[i],(void*)&thread_function,NULL);
  }
  for(i=0;i<SIZE;i++)
  { 
    gtthread_join(philosophers[i],NULL);
  }
  return EXIT_SUCCESS;
}


