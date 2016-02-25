#include "shmem.h"
#include <stdlib.h>
#include <stdio.h>

char const * const SHARED_MEMORY_PATH = "/tmp";

int getShMem(int key, size_t size, void** shm) {
  int first = 1;
  int token = ftok(SHARED_MEMORY_PATH, key);
  int id = shmget(token, size, IPC_CREAT | IPC_EXCL | 0644);
  if (id < 0) {
    first = 0;
    id = shmget(token, size, 0644);
    if (id < 0) {
      perror("Could not get shmem");
    }
  }
  *shm = shmat(id, NULL, 0);
  printf("shm(%d), token: 0x%x, addr: %p\n", id, token, *shm);
  return first;
}

void rmShMem(int key, void* pmem) {
  if (shmdt(pmem) < 0)
    perror("Could not detach shm");
  int token = ftok(SHARED_MEMORY_PATH, key);
  int id = shmget(token, 0, 0);
  if (shmctl(id, IPC_RMID, NULL) < 0) {
    printf("token: 0x%x ", token);
    perror("Could not remove shm");
  }
}
