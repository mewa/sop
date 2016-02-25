#ifndef GAME__PROTOCOL__H
#define GAME__PROTOCOL__H
#include "game_world.h"

typedef struct game_status {
  int resources;
  int wins;
  army_t army;
} game_status_t;

typedef struct train_units {
  int player_id;
  int type;
  int count;
} training_t;

typedef struct attack {
  short attacker_id;
  army_t army;
} attack_t;

typedef struct attack_result {
  short winner_id;
} attack_result_t;

typedef struct game_result {
  short winner_id;
  short winner_score;
  short defeated_score;
} game_result_t;

typedef enum action {
  CONNECT = 1,
  GAME_START = 2,
  GAME_STATUS = 3,
  UNIT_TRAINING = 4,
  ATTACK = 5,
  ATTACK_RESULT = 6,
  GAME_RESULT = 7
} action_t;

typedef struct game_msg {
  action_t action_type;
  union {
    int client_id;
    game_status_t status;
    training_t training;
    attack_t attack;
    attack_result_t attack_result;
    game_result_t game_result;
  } data;
} game_message_t;

typedef struct msgbuf {
  long mtype;
  game_message_t mdata;
} server_message_t;

#endif
