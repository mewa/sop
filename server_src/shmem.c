#include "shmem.h"
#include <stdlib.h>

char const * const SHARED_MEMORY_PATH = "/tmp";

int getShMem(int key, size_t size, void** shm) {
  int first = 1;
  int token = ftok(SHARED_MEMORY_PATH, key);
  int id = shmget(token, size, IPC_CREAT | IPC_EXCL);
  if (id < 0) {
    first = 0;
  }
  *shm = shmat(id, NULL, 0);
  return first;
}
