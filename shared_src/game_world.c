#include <string.h>
#include "game_world.h"

const int LIGHT_COST = 100;
const int HEAVY_COST = 250;
const int CAVALRY_COST = 550;
const int WORKERS_COST = 150;

const int LIGHT_TIME = 2;
const int HEAVY_TIME = 3;
const int CAVALRY_TIME = 5;
const int WORKERS_TIME = 2;

const float LIGHT_ATTACK = 1.0;
const float HEAVY_ATTACK = 1.5;
const float CAVALRY_ATTACK = 3.5;
const float WORKERS_ATTACK = 0;

const float LIGHT_DEFENCE = 1.2;
const float HEAVY_DEFENCE = 3;
const float CAVALRY_DEFENCE = 1.2;
const float WORKERS_DEFENCE = 0;

army_type_t get_army_type(const char *type_str) {
  if (strcmp(type_str, "light") == 0) {
    return LIGHT;
  } else if (strcmp(type_str, "heavy") == 0) {
    return HEAVY;
  } else if (strcmp(type_str, "cavalry") == 0) {
    return CAVALRY;
  } else if (strcmp(type_str, "worker") == 0) {
    return WORKERS;
  }

  return NONE;
}

int unit_cost(army_type_t type) {
  switch (type) {
    case LIGHT: return LIGHT_COST;
    case HEAVY: return HEAVY_COST;
    case CAVALRY: return CAVALRY_COST;
    case WORKERS: return WORKERS_COST;
    default: return 0;
  }
}

int unit_training_time(army_type_t type) {
  switch (type) {
    case LIGHT: return LIGHT_TIME;
    case HEAVY: return HEAVY_TIME;
    case CAVALRY: return CAVALRY_TIME;
    case WORKERS: return WORKERS_TIME;
    default: return 0;
  }
}

char* unit_string(army_type_t type) {
  switch (type) {
    case LIGHT: return "light";
    case HEAVY: return "heavy";
    case CAVALRY: return "cavalry";
    case WORKERS: return "workers";
    default: return "";
  }
}

float unit_attack(army_type_t type) {
  switch (type) {
    case LIGHT: return LIGHT_ATTACK;
    case HEAVY: return HEAVY_ATTACK;
    case CAVALRY: return CAVALRY_ATTACK;
    default: return 0;
  }
}

float unit_defence(army_type_t type) {
  switch (type) {
    case LIGHT: return LIGHT_DEFENCE;
    case HEAVY: return HEAVY_DEFENCE;
    case CAVALRY: return CAVALRY_DEFENCE;
    default: return 0;
  }
}

float army_attack(army_t army) {
  return (army.light * LIGHT_ATTACK) +
         (army.heavy * HEAVY_ATTACK) +
         (army.cavalry * CAVALRY_ATTACK);
}

float army_defence(army_t army) {
  return (army.light * LIGHT_DEFENCE) +
         (army.heavy * HEAVY_DEFENCE) +
         (army.cavalry * CAVALRY_DEFENCE);
}
