#pragma once

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>

char const * const SHARED_MEMORY_PATH;

int getShMem(int key, size_t size, void** shm);
