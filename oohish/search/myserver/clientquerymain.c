#include <stdio.h>

#include "client.h"


int main(int argc, char *argv[])
{
  if ( argc != 2 )
    {
      printf( "usage: %s string", argv[0] );
    }
  else 
    {
      query_string(argv[1]);
    }
}
