#pragma once

#include <pthread.h>
#include <semaphore.h>
#include <stdatomic.h>
#include <stdint.h>

// Doubly linked list for storing tasks as pushed into a task queue.
typedef struct b_pool_task {
  void* data;
  void (*procedure)(void*);
  struct b_pool_task* next;
  struct b_pool_task* prev;
} b_pool_task;

// Thread pool for statically managing tasks and threading data.
typedef struct b_pool_t {
  int num_threads;         // the maximum number of threads to spawn at any time.
  volatile int alive;      // t if running, f if it is time to free the pool.
  pthread_mutex_t tq_lock; // lock for editing the head and tail of the linked list.
  sem_t task_ready_sem;    // semaphore for signaling that tasks are ready.
  b_pool_task* tq_front;   // linked list start.
  b_pool_task* tq_end;     // linked list end.
  pthread_t* threads;      // handles to threads
} b_pool_t;

/*!
 * Allocates and launches the thread pool such that tasks can be pushed with b_pool_push_task.
 * returns NULL on failure or an allocated thread pool on success.
 * 
 * @param[in] num_threads The number of threads to spawn.
 */
b_pool_t* b_pool_init(int num_threads);

/*!
 * Launches the task, called on the parameter on the passed thread pool. you are 
 * responsible for freeing your own memory inside the procedure.
 * 
 * @param[in] thread_pool The pool to launch the task on.
 * @param[in] data The data to pass as an argument to the procedure.
 * @param[in] proc the procedure to call on the data. 
 */
void b_pool_push_task(b_pool_t* thread_pool, void* data, void (*proc)(void*));

/*!
 * Kills the pool, blocking until all tasks finish.
 * 
 * @param[in] thread_pool The pool to kill.
 */
int b_pool_free(b_pool_t* thread_pool);
