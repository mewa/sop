#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/msg.h>
#include <unistd.h>
#include "mutex.h"
#include "queue.h"
#include "player.h"
#include <signal.h>

#define NUM_PLAYERS 2

int clients = 0;
int playerIds[NUM_PLAYERS] = {0};

void dc();
void die();

void dc() {
  printf("disconnect\n");
  server_message_t msg;
  msg.mdata.action_type = CONNECT;
  msg.mdata.data.client_id = -1;
  send_message(&msg, PLAYER1);
  send_message(&msg, PLAYER2);
}

int get_player(int id) {
  int i;
  for (i = 0; i < NUM_PLAYERS; ++i) {
    if (playerIds[i] == id)
      return i + 1 + SERVER;
  }
  printf("Player %d not found\n", id);
  die();
  return -1;
}

void die(int sig) {
  printf("cleanup %d\n", sig);
  dc();
  destroy_queue();
  destroy_game();
  signal(SIGTERM, SIG_DFL);
  kill(0, SIGTERM);
}

int main(int argc, char** argv) {
  get_queue();
  if (!fork()) {
    signal(SIGTERM, die);

    server_message_t msg;
    while (receive_message(&msg, SERVER) >= 0) {
      switch (msg.mdata.action_type) {
      case CONNECT:
	printf("Client %d connected\n", msg.mdata.data.client_id);
	playerIds[clients++] = msg.mdata.data.client_id;
	send_message(&msg, get_player(msg.mdata.data.client_id));
	if (clients >= 2) {
	  msg.mdata.action_type = GAME_START;
	  send_message(&msg, SERVER);
	}
	break;
      case GAME_START:
	start_game();
	server_message_t client_msg;
	client_msg.mdata.action_type = GAME_START;
	/* send_message(); */
	break;
      default:
	break;
      }
    }
  } else {
  }
  return 0;
}
