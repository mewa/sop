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
      printf("Player%d started training <%s>\n", training.player_id - 1, unit_string(training.type));
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

void attack(attack_t attack) {
  if (!fork()) {
    int attacker = attack.attacker_id;
    int defender = attacker == 1 ? 2 : 1;

    printf("Player%d vs Player%d\n", attacker, defender);
    
    mutex_lock(mutex[attacker - 1]);
    players[attacker - 1].army.light -= attack.army.light;
    players[attacker - 1].army.heavy -= attack.army.heavy;
    players[attacker - 1].army.cavalry -= attack.army.cavalry;
    mutex_unlock(mutex[attacker - 1]);
    
    // gather defenders
    mutex_lock(mutex[defender - 1]);
    game_status_t* player = &players[defender - 1];
    army_t defender_army = player->army;
    player->army.light = 0;
    player->army.heavy = 0;
    player->army.cavalry = 0;
    mutex_unlock(mutex[defender - 1]);

    sleep(5);

    float defender_atk = army_attack(defender_army);
    float defender_def = army_defence(defender_army);
    float attacker_atk = army_attack(attack.army);
    float attacker_def = army_defence(attack.army);
    
    server_message_t msg;
    msg.mdata.data.attack_result.winner_id = attacker_atk > defender_def ? attacker : defender;
    send_message(&msg, attacker + SERVER);

    attack.army.light -= attack.army.light * (defender_atk / attacker_def);
    attack.army.heavy -= attack.army.heavy * (defender_atk / attacker_def);
    attack.army.cavalry -= attack.army.cavalry * (defender_atk / attacker_def);

    defender_army.light = defender_army.light * (attacker_atk / defender_def);
    defender_army.heavy = defender_army.heavy * (attacker_atk / defender_def);
    defender_army.cavalry = defender_army.cavalry * (attacker_atk / defender_def);

    defender_army.light = defender_army.light > 0 ? defender_army.light : 0;
    defender_army.heavy = defender_army.heavy > 0 ? defender_army.heavy : 0;
    defender_army.cavalry = defender_army.cavalry > 0 ? defender_army.cavalry : 0;

    attack.army.light = attack.army.light > 0 ? attack.army.light : 0;
    attack.army.heavy = attack.army.heavy > 0 ? attack.army.heavy : 0;
    attack.army.cavalry = attack.army.cavalry > 0 ? attack.army.cavalry : 0;
    
    
    if (attacker_atk > defender_def) {
      defender_army.light = 0;
      defender_army.heavy = 0;
      defender_army.cavalry = 0;
    }
    if (defender_atk > attacker_atk) {
      attack.army.light = 0;
      attack.army.heavy = 0;
      attack.army.cavalry = 0;
    }

    mutex_lock(mutex[defender - 1]);
    player->army = defender_army;
    mutex_unlock(mutex[defender - 1]);
    
    mutex_lock(mutex[attacker - 1]);
    player = &players[attacker - 1];

    player->wins += msg.mdata.data.attack_result.winner_id == attacker ? 1 : 0;
    
    player->army.light += attack.army.light;
    player->army.heavy += attack.army.heavy;
    player->army.cavalry += attack.army.cavalry;

    printf("Player%d vs Player%d result: Player%d %s\n",
	   attacker, defender, attacker, msg.mdata.data.attack_result.winner_id == attacker ? "won" : "lost");
    
    if (player->wins >= 5) {
      server_message_t final_msg;
      final_msg.mdata.action_type = GAME_RESULT;
      final_msg.mdata.data.game_result.winner_id = attacker;
      final_msg.mdata.data.game_result.winner_score = player->wins;
      final_msg.mdata.data.game_result.defeated_score = players[defender - 1].wins;

      broadcast_message(&final_msg);
    }
    
    mutex_unlock(mutex[attacker - 1]);
  }
}

void destroy_game() {
  rmShMem(1, players);
  mutex_destroy(mutex[0]);
  mutex_destroy(mutex[1]);
  mutex_destroy(train_mutex[0]);
  mutex_destroy(train_mutex[1]);
}
