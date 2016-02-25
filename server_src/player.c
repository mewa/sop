#include "player.h"
#include "shmem.h"

int const BASE_RESOURCE_RATE = 50;

void start_game() {
  getShMem(13, sizeof(game_status_t) * 2, (void*) &players);
  players[0].resources = 300;
  players[1].resources = 300;
}

void generate_resources(int playerId) {
  mutex_t* mutex = mutex_get(playerId);
  while (1) {
    mutex_lock(mutex);
    players[playerId].resources += BASE_RESOURCE_RATE;
  }
}
