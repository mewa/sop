#pragma once

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>

extern char const * const MUTEX_PATH;

typedef struct {
  int id;
} mutex_t;

mutex_t* mutex_get(int key);
void mutex_lock(mutex_t* mutex);
void mutex_unlock(mutex_t* mutex);
void mutex_destroy(mutex_t* mutex);
