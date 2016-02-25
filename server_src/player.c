#include "player.h"
#include "shmem.h"
#include "mutex.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>

int const BASE_RESOURCE_RATE = 50;

game_status_t* players = NULL;
mutex_t* mutex[2];

void start_game() {
  if (getShMem(1, sizeof(game_status_t) * 2, (void*) &players)) {
    memset(players, 0, sizeof(game_status_t) * 2);
    
    mutex[0] = mutex_get(0);
    mutex_lock(mutex[0]);
    players[0].resources = 300;
    mutex_unlock(mutex[0]);

    mutex[1] = mutex_get(1);
    mutex_lock(mutex[1]);
    players[1].resources = 300;
    mutex_unlock(mutex[1]);

    generate_resources(0);
    generate_resources(1);
  }
  printf("plapla: %p\n", players);
  printf("Game started\n");
}

void generate_resources(int playerId) {
  if (!fork()) {
    mutex_t* mtx = mutex[playerId];
    while (1) {
      mutex_lock(mtx);
      game_status_t player = players[playerId];
      player.resources += BASE_RESOURCE_RATE + player.army.workers * 5;
      mutex_unlock(mtx);
      
      sleep(1);
    }
  }
}

void destroy_game() {
  rmShMem(1, players);
  mutex_destroy(mutex[0]);
  mutex_destroy(mutex[1]);
}
