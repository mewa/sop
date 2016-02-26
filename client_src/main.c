#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <unistd.h>
#include <stdio.h>
#include "queue.h"
#include <signal.h>
#include "game_protocol.h"
#include "shmem.h"
#include <sys/wait.h>
#include "mutex.h"

pid_t pid;
mutex_t* client_mutex;
game_status_t* status;
int client_id;

void done(int sig) {
  if (waitpid(pid, NULL, WNOHANG)) {
    mutex_destroy(client_mutex);
    rmShMem(client_id + 1337, status);
    printf("done %d\n", sig);
    exit(1);
  }
}

int main(int argc, char** argv) {
  int const id = atoi(argv[1]);
  int const qid = id + 1;

  client_id = id;
  get_queue();
  server_message_t msg;
  client_mutex = mutex_get(id + 1337);
  msg.mdata.action_type = CONNECT;
  msg.mdata.data.client_id = id;
  send_message(&msg, SERVER);
  
  pid = fork();
  getShMem(id + 1337, sizeof(game_status_t), (void*) &status);
  if (!pid) {
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
      case GAME_START:
	printf("Game started\n");
	break;
      case GAME_STATUS:;
	mutex_lock(client_mutex);
	*status = msg.mdata.data.status;
	mutex_unlock(client_mutex);
	printf("\033[2J\033[1;1H");
	printf("\n| Attack %.2f | Defense %.2f |\n", army_attack(status->army), army_defence(status->army));
	printf("| Resources %d | Wins %d | LInf %d | HInf %d | Cav %d | Workers %d |\n",
	       status->resources, status->wins, status->army.light, status->army.heavy, status->army.cavalry, status->army.workers);
	printf("--------------------------------------------------------------------\nOrders: \n");
	printf("1) Train\n");
	printf("2) Attack\n");
	break;
      case ATTACK_RESULT:;
	int attack_winner = msg.mdata.data.attack_result.winner_id;
	printf("Player %d has won the battle\n", attack_winner);
	sleep(3);
	break;
      case GAME_RESULT:;
	int winner = msg.mdata.data.game_result.winner_id;

	if (winner == id) {
	  printf("Congratulations! You rekt 'em!\n");
	} else {
	  printf("Defeat...\n");
	}
	sleep(5);
	exit(0);
	break;
      default:
	break;
      }
    }
  } else {
    signal(SIGCHLD, done);
    int code = 0;
    while (code != 9) {
      scanf("%d", &code);
      switch (code) {
      case 1:
	kill(pid, SIGSTOP);

	printf("1) LInf(%d) attack(%.2f) defense(%.2f) training time(%d)\n",
	       unit_cost(LIGHT), unit_attack(LIGHT), unit_defence(LIGHT), unit_training_time(LIGHT));
	printf("2) HInf(%d) attack(%.2f) defense(%.2f) training time(%d)\n",
	       unit_cost(HEAVY), unit_attack(HEAVY), unit_defence(HEAVY), unit_training_time(HEAVY));
	printf("3) Cav(%d) attack(%.2f) defense(%.2f) training time(%d)\n",
	       unit_cost(CAVALRY), unit_attack(CAVALRY), unit_defence(CAVALRY), unit_training_time(CAVALRY));
	printf("4) Worker(%d) attack(%.2f) defense(%.2f) training time(%d)\n",
	       unit_cost(WORKERS), unit_attack(WORKERS), unit_defence(WORKERS), unit_training_time(WORKERS));
	

	scanf("%d", &code);
	if (code == 5) {
	  kill(pid, SIGCONT);
	  code = 0;
	  break;
	}

	server_message_t msg;
	msg.mdata.action_type = UNIT_TRAINING;
	msg.mdata.data.training.player_id = id;
	msg.mdata.data.training.type = code;
	if (code < 1 || code > 4) {
	  kill(pid, SIGCONT);
	  code = 0;
	  break;
	}
	printf("Quantity: ");
	scanf("%d", &code);
	if (code > 0) {
	  msg.mdata.data.training.count = code;
	  mutex_lock(client_mutex);
	  size_t total_cost = unit_cost(msg.mdata.data.training.type) * code;
	  printf("Total cost: %d x %d = %lu\n", code, unit_cost(msg.mdata.data.training.type), total_cost);
	  if (total_cost <= status->resources)
	    send_message(&msg, SERVER);
	  else
	    printf("Not enough resources!\n");
	  mutex_unlock(client_mutex);
	  code = 0;
	  kill(pid, SIGCONT);
	}
   	break;
      case 2:
	kill(pid, SIGSTOP);
	
	int linf, hinf, cav;
	printf("Select attacking forces:\n");
	printf("LInf: ");
	scanf("%d", &linf);
	printf("HInf: ");
	scanf("%d", &hinf);
	printf("Cav: ");
	scanf("%d", &cav);

	army_t attacker_army;
	attacker_army.light = linf;
	attacker_army.heavy = hinf;
	attacker_army.cavalry = cav;
	
	mutex_lock(client_mutex);
	if (status->army.light >= linf && status->army.heavy >= hinf && status->army.cavalry >= cav) {
	  server_message_t attack_msg;
	  attack_msg.mdata.action_type = ATTACK;
	  attack_msg.mdata.data.attack.army = attacker_army;
	  attack_msg.mdata.data.attack.attacker_id = id;
	  send_message(&attack_msg, SERVER);
	} else {
	  printf("Not enough troops!\n");
	}
	mutex_unlock(client_mutex);
	kill(pid, SIGCONT);
	break;
      }
    }
  }
  return 0;
}
