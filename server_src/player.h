#pragma once

#include <unistd.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <sys/shm.h>
#include "game_protocol.h"

extern int const BASE_RESOURCE_RATE;

void generate_resources(int playerId);
void start_game();
void destroy_game();
void train_units(training_t training);
