# BunglePool

simple general purpose threadpool for practice and fun.

```C

// spawn threads
b_pool_t* pool = b_pool_init(8);

int data = 900;
// push to the queue
b_pool_push_task(pool, (void*)&data, &some_fn);

// wait till the tasks are done.
b_pool_free(pool);

```



