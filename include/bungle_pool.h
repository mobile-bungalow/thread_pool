#pragma once

#include <pthread.h>
#include <semaphore.h>
#include <stdatomic.h>
#include <stdint.h>

typedef struct b_pool_task {
  void* data;
  void (*procedure)(void*);
  struct b_pool_task* next;
} b_pool_task;

typedef struct b_pool_t {
  int num_threads;                   // the maximum number of threads to spawn at any time.
  volatile _Atomic int pool_running; // 1 if running, 0 if it is time to free the pool.
  pthread_mutex_t list_lock;         // lock for editing the head and tail of the linked list.
  b_pool_task* task_queue_front;     // linked list start.
  b_pool_task* task_queue_end;       // linked list end.
  sem_t task_ready_sem;              // semaphore for signaling that tasks are ready.
  sem_t thread_ready_sem;            // semaphore for signaling that threads are ready.
} b_pool_t;

int b_pool_init(b_pool_t* thread_pool, int num_threads);
void b_pool_push_task(b_pool_t* thread_pool, void* data, void (*procedure)(void*));
void b_pool_wait(b_pool_t* thread_pool);
void b_pool_free(b_pool_t* thread_pool);
