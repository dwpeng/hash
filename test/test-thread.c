#include "hash.h"
#include "thpool/thpool.h"
#include <bits/pthreadtypes.h>
#include <pthread.h>
#include <stdio.h>

typedef struct {
  hashtable_ii_t* h;
  int start;
  int end;
  int task_id;
  pthread_mutex_t* lock;
} data_t;

void*
thread_insert(data_t* data)
{
  pthread_mutex_lock(data->lock);
  printf("Task %d: inserting from %d to %d, thread_id: %ld\n", data->task_id,
         data->start, data->end, pthread_self());
  pthread_mutex_unlock(data->lock);
  hashtable_ii_entry_t entry = { 0 };
  for (int i = data->start; i < data->end; i++) {
    entry.key = i;
    entry.value = i;
    hashtable_ii_put(data->h, &entry);
  }
  return NULL;
}

int
main()
{
  threadpool thpool = thpool_init(2);
  int ntasks = 16;
  pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
  data_t** tasks = malloc(ntasks * sizeof(data_t*));
  for (int i = 0; i < ntasks; i++) {
    tasks[i] = malloc(sizeof(data_t));
    tasks[i]->h = hashtable_ii_init(100, 1);
    tasks[i]->start = i * 250000;
    tasks[i]->end = (i + 1) * 250000;
    tasks[i]->task_id = i;
    tasks[i]->lock = &lock;
  }
  for (int i = 0; i < ntasks; i++) {
    thpool_add_work(thpool, (void*)thread_insert, tasks[i]);
  }
  thpool_wait(thpool);
  thpool_destroy(thpool);
  for (int i = 0; i < ntasks; i++) {
    hashtable_free(ii, tasks[i]->h);
    free(tasks[i]);
  }
  free(tasks);
}
