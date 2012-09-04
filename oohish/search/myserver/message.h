/*
 * Structs for request and response messages.
 *
 */

/* Request message */
struct request_msg {
  char name[20];
  char content[500];
  int a;
};
typedef struct request_msg request_msg_t;

/* Response message */
struct response_msg {
  char name[20];
  int a;
  int b;
};
typedef struct response_msg response_msg_t;
