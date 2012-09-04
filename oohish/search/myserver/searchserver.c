/* 
 * Start a server to handle search requests.
 * 
 * */

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>

#include "searchserver.h"
#include "cthpool/thpool.h"
#include "mysuffixtree/mygensuffixtree.h"
#include "mysuffixtree/genoperations.h"
#include "message.h"


/* Start server */
void start_server()
{
  int s;
  int success;
  struct sockaddr_un local;
  thpool_t* threadpool;             /* make a new thread pool structure     */
  gen_node_t *tree;
  tree = create_gen_suffix_tree();
  threadpool=thpool_init(4);        /* initialise it to 4 number of threads */

  printf("Server started...\n");

  if ((s = socket(AF_UNIX, SOCK_STREAM, 0)) == -1)
    {
      perror("socket");
      exit(1);
    }

  success = listen_local_sock(s, local, SOCK_PATH);
  if(success){
    for(;;)
      {
	accept_request(s, threadpool, tree);
      }
  }
  puts("Will kill threadpool");
  thpool_destroy(threadpool);
}


/* Listen local socket */
int listen_local_sock(int sock_addr, struct sockaddr_un sock, char *sock_path)
{
  int len;
  sock.sun_family = AF_UNIX;
  strcpy(sock.sun_path, sock_path);
  unlink(sock.sun_path);
  len = strlen(sock.sun_path) + sizeof(sock.sun_family);
  if (bind(sock_addr, (struct sockaddr *)&sock, len) == -1)
    {
      perror("bind error");
      return -1;
    }
  if (listen(sock_addr, 5) == -1)
    {
      perror("listen error");
      return -1;
    }
  //printf("Listening...\n");
  return 1;
}


/* Accept a connection to recieve a request */
int accept_request(int sock_addr, thpool_t* thp, gen_node_t *tree)
{
  int t;
  int s2;
  struct sockaddr_un remote;
  task_data_t *td;
  t = sizeof(remote);
  if ((s2 = accept(sock_addr, (struct sockaddr *)&remote, &t)) == -1)
    {
      perror("accept");
      exit(1);
    }
  td = create_task_data(s2, tree);
  //printf("Connected.\n");
  thpool_add_work(thp, (void*)conn_task, (void*)td);
  return 1;
}


/* Handle a client connection task */
void conn_task(task_data_t *td)
{
  //printf("# Thread working: %u\n", (int)pthread_self());
  //printf("This task has sock_addr:%d\n", td->s);
  send_ready(td->s);
  recv_task(td->s, td->tree);


  close(td->s);
  free_task_data(td);
}


/* Send a response to the client */
int send_response(int sock_addr, char *name, int a, int b)
{
  response_msg_t rsp;
  strncpy(rsp.name, name, sizeof(rsp.name));
  rsp.a = a;
  rsp.b = b;
  if (send(sock_addr, &rsp, sizeof(rsp), 0) < 0)
    {
      perror("send ready signal error");
      return -1;
    }
  return 0;
}


/* Send ready signal to client */
int send_ready(int sock_addr)
{
  if((send_response(sock_addr, "ready", 0, 0)) != 0)
    {
      return -1;
    }
  return 0;
}


/* Recieve the task from the client */
int recv_task(int sock_addr, gen_node_t *tree)
{
  int t;
  request_msg_t rqst;
  gen_index_list_t *leaf_pairs;
  if ((t=recv(sock_addr, &rqst, sizeof(rqst), 0)) > 0)
    {
      //printf("task type: %s\n", rqst.name);
      if(strcmp(rqst.name, "query_string") == 0)
	{
	  printf("Query string: %s\n", rqst.content);
	  leaf_pairs = get_substring_indices(tree, rqst.content);
	  send_query_response(sock_addr, leaf_pairs);	  
	}
      else if(strcmp(rqst.name, "add_string") == 0)
	{
	  printf("Add string: %s, id: %d\n", rqst.content, rqst.a);
	  add_string_gen_suffix_tree(tree, rqst.content, rqst.a);
	  send_add_string_response(sock_addr);
	}
      else if(strcmp(rqst.name, "swap_tree") == 0)
	{
	}
    }
  return 0;
}


/* Create task data */
task_data_t *create_task_data(int sock_addr, gen_node_t *tree)
{
  task_data_t *td;
  if((td = malloc(sizeof *td)) == NULL)
    {
      perror("malloc task data fail.\n");
      exit(1);
    }
  td->s = sock_addr;
  td->tree = tree;
  return td;
}


/* Free task data */
void free_task_data(task_data_t *td)
{
  free(td);
}


/* Send add_string success signal to client */
void send_add_string_response(int sock_addr)
{
  send_response(sock_addr, "add_string_success", 0, 0);
}


/* Send the response to the query. */
void send_query_response(int sock_addr, gen_index_list_t *lp)
{
  request_msg_t rqst;
  int t;
  while(lp != NULL)
    {
      send_response(sock_addr, "query_response", lp->string_id, lp->index);

      if ((t=recv(sock_addr, &rqst, sizeof(rqst), 0)) > 0)
	{
	  //printf("rqst name: %s\n", rqst.name);
	}
      lp = lp->next;
    }
  send_response(sock_addr, "finished_query", 0, 0);
}
