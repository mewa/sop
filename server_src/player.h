#pragma once

#include <unistd.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <sys/shm.h>
#include "mutex.h"
#include "game_protocol.h"

game_status_t* players;

int const BASE_RESOURCE_RATE;

void generate_resources(int playerId);
void start_game();
