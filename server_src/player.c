#include "player.h"
#include "shmem.h"
#include "mutex.h"
#include "queue.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>

int const BASE_RESOURCE_RATE = 50;

game_status_t* players = NULL;
mutex_t* mutex[2];
mutex_t* train_mutex[2];

void start_game() {
  if (getShMem(1, sizeof(game_status_t) * 2, (void*) &players)) {
    memset(players, 0, sizeof(game_status_t) * 2);

    train_mutex[0] = mutex_get(66);
    train_mutex[1] = mutex_get(67);
    
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
  printf("[Server] Game started\n");
}

void generate_resources(int playerId) {
  if (!fork()) {
    mutex_t* mtx = mutex[playerId];
    while (1) {
      mutex_lock(mtx);
      game_status_t* player = &players[playerId];
      player->resources += BASE_RESOURCE_RATE + player->army.workers * 5;
      server_message_t msg;
      msg.mdata.action_type = GAME_STATUS;
      msg.mdata.data.status = players[playerId];
      send_message(&msg, playerId + SERVER + 1);
      mutex_unlock(mtx);
      sleep(1);
    }
  }
}

void train_units(training_t training) {
  if (!fork()) {
    printf("[Server] Client %d started training %d %s\n",
	   training.player_id, training.count, unit_string(training.type));
    mutex_t* mtx = mutex[training.player_id - 1];
    mutex_t* train_mtx = train_mutex[training.player_id - 1];
    
    mutex_lock(mtx);
    game_status_t* player = &players[training.player_id - 1];
    player->resources -= training.count * unit_cost(training.type);
    mutex_unlock(mtx);

    mutex_lock(train_mtx);
    int i;
    for (i = 0; i < training.count; ++i) {
      sleep(unit_training_time(training.type));
      mutex_lock(mtx);
      printf("Player%d train type: %d\n", training.player_id - 1, training.type);
      switch (training.type) {
      case LIGHT:
	player->army.light += 1;
	break;
      case HEAVY:
	player->army.heavy += 1;
	break;
      case CAVALRY:
	player->army.cavalry += 1;
	break;
      case WORKERS:
	player->army.workers += 1;
	break;
      }
      mutex_unlock(mtx);
    }
    mutex_unlock(train_mtx);
  }
}

void destroy_game() {
  rmShMem(1, players);
  mutex_destroy(mutex[0]);
  mutex_destroy(mutex[1]);
  mutex_destroy(train_mutex[0]);
  mutex_destroy(train_mutex[1]);
}
