#include "queue.h"
#include <stdio.h>

char const * const QUEUE_PATH = "/tmp";
int const SERVER_QUEUE_ID = 19;

int const SERVER = 1;
int const PLAYER1 = 2;
int const PLAYER2 = 3;

int gQueueId;

void get_queue() {
  gQueueId = msgget(ftok(QUEUE_PATH, SERVER_QUEUE_ID), IPC_CREAT | 0644);
  if (gQueueId < 0)
    perror("Could not get queue");
}

void destroy_queue() {
  if (msgctl(gQueueId, IPC_RMID, NULL) < 0)
    perror("Could not remove queue");
}

int receive_message(server_message_t* msg, int type) {
  return msgrcv(gQueueId, msg, sizeof(game_message_t), type, 0);
}

int send_message(server_message_t* msg, int type) {
  msg->mtype = type;
  return msgsnd(gQueueId, msg, sizeof(game_message_t), 0);
}
