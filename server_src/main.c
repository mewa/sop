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

void die() {
  destroy_queue();
  signal(SIGINT, SIG_DFL);
  kill(0, SIGINT);
}

int main(int argc, char** argv) {
  get_queue();
  if (!fork()) {
    int clients = 0;
    int playerIds[2] = {0};
    server_message_t msg;
    while (receive_message(&msg, SERVER)) {
      switch (msg.mdata.action_type) {
      case CONNECT:
	printf("Client %d connected\n", msg.mdata.data.client_id);
	playerIds[clients++] = msg.mdata.data.client_id;
	if (clients >= 2) {
	  msg.mdata.action_type = GAME_START;
	  send_message(&msg, SERVER);
	}
	break;
      case GAME_START:
	printf("Game started\n");
	start_game();
	server_message_t client_msg;
	client_msg.mdata.action_type = GAME_STATUS;
	if (fork()) {
	  generate_resources(0);
	} else {
	  generate_resources(1);
	}
	break;
      }
    }
  } else {
    signal(SIGINT, die);
  }
  return 0;
}
