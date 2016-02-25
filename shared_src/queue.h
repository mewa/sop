#pragma once

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include "game_protocol.h"

extern char const * const QUEUE_PATH;
extern int const SERVER_QUEUE_ID;

extern int const SERVER;
extern int const PLAYER1;
extern int const PLAYER2;

void get_queue();
void destroy_queue();
int receive_message(server_message_t* msg, int type);
int send_message(server_message_t* msg, int type);
