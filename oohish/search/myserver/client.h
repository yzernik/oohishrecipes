#include <sys/un.h>

#define SOCK_PATH "/tmp/connect_socket"


/* Linked list */
struct result_list {
  struct result_node *head;
  struct result_node *tail;
};
typedef struct result_list result_list_t;

/* Linked list node */
struct result_node {
  int val;
  struct result_node *next;
};
typedef struct result_node result_node_t;


/* Connect socket */
int connect_sock(int sock_addr, struct sockaddr_un sock, char *sock_path);

/* Recieve ready signal */
int recv_ready_signal(int sock_addr);

/* Send request to server */
int send_request(int sock_addr, char *tt, char *str, int str_id);
