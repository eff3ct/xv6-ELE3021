#include "types.h"
#include "user.h"

thread_t t[10];

void* 
thread_func(void* i) {
  int n = *(int*)i;
  printf(1, "thread %d\n", n);
  thread_exit((void*)0);
  return (void*)0;
}

void*
overlapped_thread(void* i) {
  int status;
  int n = *(int*)i;
  int c = n + 1;
  printf(1, "thread parent %d\n", n);
  thread_create(&t[1], thread_func, (void*)&c);
  thread_join(t[1], (void**)&status);
  printf(1, "thread child joined. %d\n", n);
  thread_exit((void*)0);
  return (void*)0;
}

int 
main() 
{
  int a[10] = { 0, 1, 2, 3, 4, 5, 6, 7 ,8, 9 };
  for(int i = 0; i < 10; ++i) {
    thread_create(&t[i], thread_func, (void*)&a[i]);
  }

  int status;
  for(int i = 0; i < 10; ++i) {
    thread_join(t[i], (void**)&status);
    printf(1, "thread %d joined.\n", i);
  }

  printf(1, "main ended.\n");

  printf(1, "overlapped-thread-test started.\n");
  thread_create(&t[0], overlapped_thread, (void*)&a[0]);
  thread_join(t[0], (void**)&status);
  printf(1, "overlapped-thread-test ended.\n");

  exit();
}