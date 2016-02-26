#include "queue.h"
#include <stdio.h>
#include <stdlib.h>

int playerIds[NUM_PLAYERS] = {0};

char const * const QUEUE_PATH = "/tmp";
int const SERVER_QUEUE_ID = 19;

int const SERVER = 1;
int const PLAYER1 = 2;
int const PLAYER2 = 3;

int gQueueId;

int get_player(int id) {
  int i;
  for (i = 0; i < NUM_PLAYERS + SERVER + 1; ++i) {
    if (playerIds[i] >= 0 && playerIds[i] == id)
      return i + 1 + SERVER;
  }
  printf("Player %d not found\n", id);
  exit(1);
  return -1;
}

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

void broadcast_message(server_message_t* msg) {
  int i;
  for (i = 2; i < NUM_PLAYERS + SERVER + 1; ++i) {
    send_message(msg, i);
  }
}
