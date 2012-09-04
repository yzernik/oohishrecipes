/* Generalized Suffix Tree program */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mygensuffixtree.h"


gen_index_list_t * insert_in_gen_index_list(gen_index_list_t *l, int string_id, int index)
{
  gen_index_list_t *this_node, *next_node, *new_node;
  if((new_node = malloc(sizeof *new_node)) == NULL)
    {
      printf("malloc fail.\n");
      return new_node;
    }
  new_node->string_id = string_id;
  new_node->index = index;
  new_node->next = NULL;
  this_node = NULL;
  next_node = l;
  while(next_node != NULL && ((string_id > next_node->string_id) || ((string_id == next_node->string_id) && (index > next_node->index))))
    {
      this_node = next_node;
      next_node = this_node->next;
    }
  
  if(!(next_node != NULL && ((string_id == next_node->string_id) && (index == next_node->index))))
    {
      new_node->next = next_node;
      if(this_node == NULL)
	{
	  return new_node;
	}
      else
	{
	  this_node->next = new_node;
	  return l;
	}
    }
  free(new_node);
  return l;
}
 
void print_gen_index_list(gen_index_list_t * l)
{
  gen_index_list_t *this_node = l;
  while(this_node != NULL)
    {
      printf("(%d, %d)\n", this_node->string_id, this_node->index);
      this_node = this_node->next;
    }
}


gen_node_t * create_gen_suffix_tree()
{
  gen_node_t *tree;
  if((tree = malloc(sizeof *tree)) == NULL)
    {
      printf("malloc fail.\n");
      return tree;
    }
  tree->edges = NULL;
  tree->indices = NULL;

  return tree;
}


void add_string_gen_suffix_tree(gen_node_t *t, char *s, int string_id)
{
  char *new_s;
  if((new_s = malloc((sizeof(char))*(strlen(s)+2))) == NULL)
    {
      printf("malloc fail.\n");
    }
  strcpy(new_s, s);
  strcat(new_s, "$");
  int i;
  for(i = 0; new_s[i] != '\0'; i++)
    {
      add_suffix_gen_suffix_tree(t, &new_s[i], string_id, i);
    }
  free(new_s);
}


void add_suffix_gen_suffix_tree(gen_node_t *t, char *s, int string_id, int start_index)
{
  gen_node_t *this_node, *next_node;
  gen_edge_list_t *next_edge;
  this_node = t;
  int i;
  for(i = 0; s[i] != '\0'; i++)
    {
      this_node->edges = insert_gen_edge(this_node->edges, s[i]);
      next_edge = find_gen_edge(this_node->edges, s[i]);

      if(next_edge->child == NULL)
	{
	  if((next_node = malloc(sizeof *next_node)) == NULL)
	    {
	      printf("malloc fail.\n");
	    }
	  next_node->edges = NULL;
	  next_node->indices = NULL;
	  next_edge->child = next_node;
	}
      next_node = next_edge->child;
      next_node->indices = insert_in_gen_index_list(next_node->indices, string_id, start_index);
      this_node = next_node;
    }
}


gen_edge_list_t * find_gen_edge(gen_edge_list_t *edge_list, char c)
{
  gen_edge_list_t *this_edge;
  this_edge = edge_list;
  while(this_edge)
    {
      if(this_edge->c == c)
	{
	  return this_edge;
	}
      this_edge = this_edge->next;
    }
  return NULL;
}

gen_edge_list_t * insert_gen_edge(gen_edge_list_t *edge_list, char c)
{
  gen_edge_list_t *this_edge, *next_edge, *new_edge;
  if((new_edge = malloc(sizeof *new_edge)) == NULL)
    {
      printf("malloc fail.\n");
      return new_edge;
    }
  new_edge->c = c;
  new_edge->child = NULL;
  new_edge->next = NULL;
  this_edge = NULL;
  next_edge = edge_list;
  while(next_edge != NULL && (c > next_edge->c))
    {
      this_edge = next_edge;
      next_edge = this_edge->next;
    }
  
  if(!(next_edge != NULL && (c == next_edge->c)))
    {
      new_edge->next = next_edge;
      if(this_edge == NULL)
	{
	  return new_edge;
	}
      else
	{
	  this_edge->next = new_edge;
	  return edge_list;
	}
    }
  else
    {
      free(new_edge);
    }
  return edge_list;
}


void free_gen_suffix_tree(gen_node_t *t)
{
  free_gen_edge_list(t->edges);
  free_gen_index_list(t->indices);
  free(t);
}


void free_gen_index_list(gen_index_list_t * l)
{
  gen_index_list_t *this_list, *next_list;
  this_list = l;
  while(this_list != NULL)
    {
      next_list = this_list->next;
      free(this_list);
      this_list = next_list;
    }
}

void free_gen_edge_list(gen_edge_list_t * edges)
{
  gen_edge_list_t *this_edge, *next_edge;
  this_edge = edges;
  while(this_edge != NULL)
    {
      free_gen_suffix_tree(this_edge->child);
      next_edge = this_edge->next;
      free(this_edge);
      this_edge = next_edge;
    }
}
