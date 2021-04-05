#include "../include/bungle_pool.h"
#include <stdlib.h>

void* handle_jobs(void* input)
{
  b_pool_t* pool = (b_pool_t*)input;
  while (1) {
    sem_wait(&(pool->task_ready_sem));

    // pop critical section
    pthread_mutex_lock(&(pool->tq_lock));
    b_pool_task* next_task = pool->tq_end;
    if (pool->tq_end == pool->tq_front) {
      pool->tq_front = NULL;
      pool->tq_end = NULL;
    } else {
      pool->tq_end = pool->tq_end->prev;
    }
    pthread_mutex_unlock(&(pool->tq_lock));

    if (next_task) {
      next_task->procedure(next_task->data);
      free(next_task);
    }

    if (!pool->alive && !pool->tq_end) {
      pthread_exit(NULL);
      return NULL;
    }
  }

  return NULL;
}

b_pool_t* b_pool_init(int num_threads)
{
  b_pool_t* thread_pool = (b_pool_t*)malloc(sizeof(b_pool_t));

  *thread_pool = (b_pool_t) {
    .alive = 1,
    .num_threads = num_threads,
    .tq_end = NULL,
    .tq_front = NULL,
    .threads = (pthread_t*)calloc(sizeof(pthread_t), num_threads)
  };

  pthread_mutexattr_t attr;
  if (pthread_mutexattr_init(&attr) == -1) {
    b_pool_free(thread_pool);
    return NULL;
  }
  if (pthread_mutex_init(&(thread_pool->tq_lock), &attr) == -1) {
    b_pool_free(thread_pool);
    return NULL;
  }
  if (sem_init(&(thread_pool->task_ready_sem), 1, 0) == -1) {
    b_pool_free(thread_pool);
    return NULL;
  }

  for (int i = 0; i < num_threads; i++) {
    pthread_create(&(thread_pool->threads)[i], NULL, &handle_jobs, thread_pool);
  }

  return thread_pool;
}

void b_pool_push_task(b_pool_t* thread_pool, void* data, void(procedure)(void*))
{
  b_pool_task* task = (b_pool_task*)malloc(sizeof(b_pool_task));

  *task = (b_pool_task) {
    .procedure = procedure,
    .data = data,
    .prev = NULL,
    .next = NULL,
  };

  // push critical section
  pthread_mutex_lock(&thread_pool->tq_lock);
  if (thread_pool->tq_end && thread_pool->tq_front) {
    thread_pool->tq_end->next = task;
    task->prev = thread_pool->tq_end;
    thread_pool->tq_end = task;
  } else {
    thread_pool->tq_end = task;
    thread_pool->tq_front = task;
    thread_pool->tq_front->next = task;
  }
  pthread_mutex_unlock(&thread_pool->tq_lock);

  sem_post(&thread_pool->task_ready_sem);
}

int b_pool_free(b_pool_t* thread_pool)
{

  thread_pool->alive = 0;
  for (int i = 0; i < thread_pool->num_threads; i++) {
    if (sem_post(&(thread_pool->task_ready_sem)) == -1) {
      return -1;
    }
  }

  for (int i = 0; i < thread_pool->num_threads; i++) {
    void * not ;
    if (pthread_join(thread_pool->threads[i], &not ) == -1) {
      return -1;
    }
    (void)not ;
  }

  if (thread_pool->tq_front || thread_pool->tq_end) {
    return -1;
  }

  pthread_mutex_destroy(&(thread_pool->tq_lock));
  sem_destroy(&(thread_pool->task_ready_sem));
  free(thread_pool->threads);
  free(thread_pool);

  return 0;
};
