#include "../include/bungle_pool.h"
#include <assert.h>
#include <stdio.h>
#include <unistd.h>

#define THREAD_CT 16
static int test_arr[THREAD_CT * THREAD_CT] = { 0 };

void modify_index(void* just_an_int)
{
  long cast_res = (long)just_an_int;
  test_arr[cast_res] = 1;
}

int main()
{
  b_pool_t* pool = b_pool_init(THREAD_CT);

  if (pool == NULL) {
    return 1;
  }

  for (long i = 0; i < THREAD_CT * THREAD_CT; i++) {
    b_pool_push_task(pool, (void*)i, &modify_index);
  }

  // this blocks until the tasks finish.
  b_pool_free(pool);

  for (long i = 0; i < THREAD_CT * THREAD_CT; i++) {
    assert(test_arr[i] == 1);
  }
}
