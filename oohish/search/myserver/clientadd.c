#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>

#include "client.h"
#include "clientadd.h"
#include "message.h"


/* Start a client to add string */
int add_string(char *str, int str_id)
{
  int s, rv;
  struct sockaddr_un remote;

  printf("str: %s, str_id: %d\n", str, str_id);
  if ((s = socket(AF_UNIX, SOCK_STREAM, 0)) == -1)
    {
      perror("socket");
      exit(1);
    }

  rv = -1;
  if((connect_sock(s, remote, SOCK_PATH)) == 0)
    {
      if((recv_ready_signal(s)) == 0)
	{
	  if((send_request(s, "add_string", str, str_id)) == 0)
	    {
	      if((recv_add_string_signal(s)) == 0)
		{
		  rv = 0;
		}
	    }
	  
	}
    }
  close(s);
  return rv;
}


/* Recieve add_string_success signal */
int recv_add_string_signal(int sock_addr)
{
  int t;
  response_msg_t rsp;
  if ((t=recv(sock_addr, &rsp, sizeof(rsp), 0)) > 0)
    {
      printf("got message:%s\n", rsp.name);
    } 
  return 0;
}
