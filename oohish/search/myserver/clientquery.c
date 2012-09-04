#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>

#include "client.h"
#include "clientquery.h"
#include "message.h"


/* Start a client to query string */
result_list_t *query_string(char *str)
{
  int s;
  struct sockaddr_un remote;
  result_list_t *rv;


  if ((s = socket(AF_UNIX, SOCK_STREAM, 0)) == -1)
    {
      perror("socket");
      exit(1);
    }

  if((connect_sock(s, remote, SOCK_PATH)) == 0)
    {
      if((recv_ready_signal(s)) == 0)
	{
	  if((send_request(s, "query_string", str, 0)) == 0)
	    {
	      rv = recv_query_result(s);
	    }
	}
    }
  close(s);
  return rv;
}



/* Recieve query_string_success signal */
result_list_t *recv_query_result(int sock_addr)
{
  int t, done;
  response_msg_t rsp;
  result_list_t *rv;
  rv = create_result_list();
  done = 0;
  while(done == 0)
    {
      if ((t=recv(sock_addr, &rsp, sizeof(rsp), 0)) > 0)
	{
	  //printf("got message:%s\n", rsp.name);
	  if((strcmp(rsp.name, "query_response")) == 0)
	    {
	      //printf("query result- string_id: %d, index: %d\n", rsp.a, rsp.b);
	      add_result(rv, rsp.a);
	      if((send_request(sock_addr, "continue_query", "", 0)) == 0)
		{
		  //printf("continuing query\n");
		}
	    }
	  else
	    {
	      done = 1;
	    }
	}
    }
  return rv;
}


/* Create new result list */
result_list_t *create_result_list()
{
  result_list_t *list = (result_list_t*)malloc(sizeof(result_list_t));
  list->head = NULL;
  list->tail = NULL;
  return list;
}

/* Add new val to result list */
void add_result(result_list_t *rl, int val)
{
  result_node_t *node = (result_node_t*)malloc(sizeof(result_node_t));
  node->val = val;
  node->next = NULL;
  if(rl->head == NULL)
    {
      rl->head = node;
      rl->tail = node;
    }
  else if(node->val > rl->tail->val)
    {
      rl->tail->next = node;
      rl->tail = node;
    }
}

/* Print result list */
void print_result_list(result_list_t *rl)
{
  result_node_t *current;
  current = rl->head;
  while(current != NULL)
    {
      printf("node val: %d\n", current->val);
      current = current->next;
    }
}
