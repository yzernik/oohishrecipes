#include <sys/un.h>

#include "cthpool/thpool.h"
#include "mysuffixtree/mygensuffixtree.h"


/* Define connect socket path */
#define SOCK_PATH "/tmp/connect_socket"

/* Contains socket descriptor and suffix tree pointer */
struct task_data {
  int s;
  struct gen_node *tree;
};
typedef struct task_data task_data_t;

/* Start server */
void start_server();

/* Listen local socket */
int listen_local_sock(int sock_addr, struct sockaddr_un sock, char *sock_path);

/* Accept a connection to recieve a request */
int accept_request(int sock_addr, thpool_t* thp, gen_node_t *tree);

/* Handle a client connection task */
void conn_task(task_data_t *td);

/* Send a response to the client */
int send_response(int sock_addr, char *name, int a, int b);

/* Send ready signal to client */
int send_ready(int sock_addr);

/* Recieve the task from the client */
int recv_task(int sock_addr, gen_node_t *tree);

/* Create task data */
task_data_t *create_task_data(int sock_addr, gen_node_t *tree);

/* Free task data */
void free_task_data(task_data_t *td);

/* Send add_string success signal to client */
void send_add_string_response(int sock_addr);

/* Send the response to the query. */
void send_query_response(int sock_addr, gen_index_list_t *lp);
