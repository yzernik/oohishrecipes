/* Suffix Tree program */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mygensuffixtree.h"
#include "genoperations.h"


int has_substring(gen_node_t *t, char *s)
{
  gen_node_t *this_node;
  gen_edge_list_t *this_edge;
  this_node = t;
  int i;
  for(i = 0; i < strlen(s); i++)
    {
      if((this_edge = find_gen_edge(this_node->edges, s[i])))
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

gen_index_list_t * get_substring_indices(gen_node_t *t, char *s)
{
  gen_node_t *this_node;
  gen_edge_list_t *this_edge;
  this_node = t;
  int i;
  for(i = 0; i < strlen(s); i++)
    {
      if((this_edge = find_gen_edge(this_node->edges, s[i])))
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
