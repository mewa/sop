#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <unistd.h>
#include <stdio.h>
#include "queue.h"

int main(int argc, char** argv) {
  int const id = atoi(argv[1]);
  int const qid = id + 1;
  get_queue();
  server_message_t msg;
  msg.mdata.action_type = CONNECT;
  msg.mdata.data.client_id = id;
  send_message(&msg, SERVER);
  if (!fork()) {
    while (receive_message(&msg, qid) >= 0) {
      switch (msg.mdata.action_type) {
      case CONNECT:
	if (msg.mdata.data.client_id != id) {
	  fprintf(stderr, "Server offline\n");
	  exit(1);
	}
	else
	  printf("Connected to server\n");
	break;
      }
    }
  } else {
    
  }
  return 0;
}
