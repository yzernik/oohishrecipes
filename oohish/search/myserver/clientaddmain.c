#include <stdio.h>

#include "client.h"


int main(int argc, char *argv[])
{
  if ( argc != 3 )
    {
      printf( "usage: %s string string_id", argv[0] );
    }
  else 
    {
      printf("str: %s, str_id: %d\n", argv[1], atoi(argv[2]));
      add_string(argv[1], atoi(argv[2]));
    }
}
