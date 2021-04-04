#include "../include/bungle_pool.h"
#include <stdlib.h>

void* handle_jobs(void* input)
{
  b_pool_t* pool = (b_pool_t*)input;
  while (1) {
    sem_wait(&pool->task_ready_sem);
    // pop the linked list.
    b_pool_task* next_task = pool->task_queue_front;
    if (next_task == pool->task_queue_end) {
      pool->task_queue_end = NULL;
    }
    pool->task_queue_front = next_task->next;
  }
  return NULL;
}

int b_pool_init(b_pool_t* thread_pool, int num_threads)
{
  int ret;
  thread_pool = (b_pool_t*)malloc(sizeof(b_pool_t));
  thread_pool->task_queue_end = NULL;
  thread_pool->task_queue_front = NULL;
  pthread_mutexattr_t attr;
  if (pthread_mutexattr_init(&attr) == -1) {
    return -1;
  }
  if (pthread_mutex_init(&thread_pool->list_lock, &attr) == -1) {
    return -1;
  }
  if (sem_init(&thread_pool->task_ready_sem, 1, 0) == -1) {
    return -1;
  }
  return 0;
}

void b_pool_push_task(b_pool_t* thread_pool, void* data, void(procedure)(void*))
{
  b_pool_task* task = (b_pool_task*)malloc(sizeof(b_pool_task));
  task->procedure = procedure;
  task->data = data;
  pthread_mutex_lock(&thread_pool->list_lock);
  if (!thread_pool->task_queue_front && !thread_pool->task_queue_end) {
    thread_pool->task_queue_end = task;
    thread_pool->task_queue_front = task;
  } else {
    thread_pool->task_queue_end->next = task;
    thread_pool->task_queue_end = task;
  }
  pthread_mutex_unlock(&thread_pool->list_lock);
  sem_post(&thread_pool->task_ready_sem);
}

void b_pool_wait(b_pool_t* thread_pool) {};
void b_pool_free(b_pool_t* thread_pool) {};
