#ifndef GAME__WORLD__H
#define GAME__WORLD__H

extern const int LIGHT_COST;
extern const int HEAVY_COST;
extern const int CAVALRY_COST;
extern const int WORKERS_COST;

extern const int LIGHT_TIME;
extern const int HEAVY_TIME;
extern const int CAVALRY_TIME;
extern const int WORKERS_TIME;

extern const float LIGHT_ATTACK;
extern const float HEAVY_ATTACK;
extern const float CAVALRY_ATTACK;
extern const float WORKERS_ATTACK;

extern const float LIGHT_DEFENCE;
extern const float HEAVY_DEFENCE;
extern const float CAVALRY_DEFENCE;
extern const float WORKERS_DEFENCE;

typedef enum army_type {
  NONE = 0,
  LIGHT = 1,
  HEAVY = 2,
  CAVALRY = 3,
  WORKERS = 4
} army_type_t;

typedef struct army {
  int light;
  int heavy;
  int cavalry;
  int workers;
} army_t;

army_type_t get_army_type(const char *type_str);
int unit_cost(army_type_t type);
int unit_training_time(army_type_t type);
char *unit_string(army_type_t type);
float army_defence(army_t army);
float army_attack(army_t army);
float unit_attack(army_type_t type);
float unit_defence(army_type_t type);

#endif
