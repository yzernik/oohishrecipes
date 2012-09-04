/* Suffix Tree program */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mysuffixtree.h"
#include "operations.h"


int has_substring(node_t *t, char *s)
{
  node_t *this_node;
  edge_list_t *this_edge;
  this_node = t;
  int i;
  for(i = 0; i < strlen(s); i++)
    {
      if((this_edge = find_edge(this_node->edges, s[i])))
	{
	  this_node = this_edge->child;
	}
      else
	{
	  return 0;
	}
    }
  return 1;
}

index_list_t * get_substring_indices(node_t *t, char *s)
{
  node_t *this_node;
  edge_list_t *this_edge;
  this_node = t;
  int i;
  for(i = 0; i < strlen(s); i++)
    {
      if((this_edge = find_edge(this_node->edges, s[i])))
	{
	  this_node = this_edge->child;
	}
      else
	{
	  return NULL;
	}
    }
  return this_node->indices;
}
