Tuxun Lu tlu32
Jiarui Chen jchen324

We use mutex to ensure the calculator instance's shared data is safe to access from multiple threads.

When a thread accesses the critical regions, we call pthread_mutex_lock to lock the mutex for the thread's exclusive access.

Only after the thread completes accessing the critical regions and calls pthread_mutex_unlock to unlock the mutex, other threads are able to access the critical regions.

The critical regions are where the shared data are modified, since all updates to calculator variables are not atomic and multiple threads could do the updates at the same time.