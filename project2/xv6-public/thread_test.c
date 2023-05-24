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

  exit();
}