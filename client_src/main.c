#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <unistd.h>
#include "queue.h"

int main(int argc, char** argv) {
  int id = atoi(argv[1]);
  get_queue();
  server_message_t msg;
  msg.mdata.action_type = CONNECT;
  msg.mdata.data.client_id = id;
  send_message(&msg, SERVER);

  return 0;
}
