Wygląd wiadomości do wysłania przez serwer

Msgbuf:
```
typedef struct msgbuf {
  long mtype;
  struct game_msg mdata;
};
```
Game Message:
```
struct game_msg {
  action_t action_type;
  union {
    int client_id;
    struct game_status status;
    struct train_units training;
  } data;
};
```
