#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>

#include "client.h"
#include "message.h"


/* Connect socket */
int connect_sock(int sock_addr, struct sockaddr_un sock, char *sock_path)
{
  int len;
  //printf("Trying to connect...\n");
  sock.sun_family = AF_UNIX;
  strcpy(sock.sun_path, sock_path);
  len = strlen(sock.sun_path) + sizeof(sock.sun_family);
  if (connect(sock_addr, (struct sockaddr *)&sock, len) == -1)
    {
      perror("connect error");
      return -1;
    }
  //printf("Connected.\n");
  return 0;
}


/* Recieve ready signal */
int recv_ready_signal(int sock_addr)
{
  int t;
  response_msg_t rsp;
  if ((t=recv(sock_addr, &rsp, sizeof(rsp), 0)) > 0)
    {
      //printf("got message:%s\n", rsp.name);
      if((strcmp(rsp.name, "ready")) == 0)
	{
	  //printf("got ready signal\n");
	}
      else{
	perror("wrong signal, not ready");
	return -1;
      }
    } 
  return 0;
}


/* Send request to server */
int send_request(int sock_addr, char *tt, char *str, int str_id)
{
  request_msg_t rqst;
  strncpy(rqst.name, tt, sizeof(rqst.name));
  strncpy(rqst.content, str, sizeof(rqst.content));
  rqst.a = str_id;
  if (send(sock_addr, &rqst, sizeof(rqst), 0) < 0)
    {
      perror("send ready signal error");
      return -1;
    }
  //printf("Sent Task.\n");
  return 0;
}
