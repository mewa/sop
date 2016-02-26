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

int clients = 0;

void dc();
void die();

void dc() {
  printf("Disconnect\n");
  server_message_t msg;
  msg.mdata.action_type = CONNECT;
  msg.mdata.data.client_id = -1;
  broadcast_message(&msg);
}

void die(int sig) {
  printf("Cleanup %d\n", sig);
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
    signal(SIGCHLD, die);
    
    server_message_t msg;
    while (receive_message(&msg, SERVER) >= 0) {
      switch (msg.mdata.action_type) {
      case CONNECT:
	printf("[Server] Client %d connected\n", msg.mdata.data.client_id);
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
	broadcast_message(&client_msg);
	break;
      case UNIT_TRAINING:
	train_units(msg.mdata.data.training);
	break;
      case ATTACK:
	attack(msg.mdata.data.attack);
	break;
      default:
	break;
      }
    }
  } else {
  }
  return 0;
}
