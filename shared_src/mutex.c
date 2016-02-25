#include "mutex.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

char const * const MUTEX_PATH = "/tmp";

mutex_t* mutex_get(int key) {
  mutex_t* mutex = malloc(sizeof(mutex_t));
  mutex->id = semget(ftok(MUTEX_PATH, key), 1, IPC_CREAT | 0600);
  if (mutex->id < 0) {
    perror("Error acquring mutex");
    exit(1);
  }
  mutex_unlock(mutex);
  return mutex;
}

void mutex_lock(mutex_t* mutex) {
  struct sembuf op;
  memset(&op, 0, sizeof(struct sembuf));
  op.sem_op = -1;
  semop(mutex->id, &op, 1);
}

void mutex_unlock(mutex_t* mutex) {
  int val = semctl(mutex->id, 0, GETVAL);
  if (val < 1) {
    struct sembuf op;
    memset(&op, 0, sizeof(struct sembuf));
    op.sem_op = 1;
    semop(mutex->id, &op, 1);
  }
}

void mutex_destroy(mutex_t* mutex) {
  printf("mutex rm id: %d\n", mutex->id);
  if (semctl(mutex->id, 0, IPC_RMID) < 0)
    perror("Could not destroy mutex");
  free(mutex);
}
