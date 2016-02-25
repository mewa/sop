#pragma once

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include "game_protocol.h"

char const * const QUEUE_PATH;
int const SERVER_QUEUE_ID;

int const SERVER;
int const PLAYER1;
int const PLAYER2;

void get_queue();
void destroy_queue();
int receive_message(server_message_t* msg, int type);
int send_message(server_message_t* msg, int type);
