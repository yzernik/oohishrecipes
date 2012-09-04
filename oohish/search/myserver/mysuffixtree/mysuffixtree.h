#ifndef SUFFIX_TREE_H_INCLUDED
#define SUFFIX_TREE_H_INCLUDED


/* linked list of edges with char and connecting child nodes */
struct edge_list {
  char c;
  struct node *child;
  struct edge_list * next;
};
typedef struct edge_list edge_list_t;

/* linked list of ints */
struct index_list {
  int val;
  struct index_list * next;
};
typedef struct index_list index_list_t;

/* node in suffix tree */
struct node {
  edge_list_t *edges;
  index_list_t *indices;
};
typedef struct node node_t;


/* builds a suffix tree for the string s 
 */
index_list_t * insert_in_index_list(index_list_t *l, int n);
void print_index_list(index_list_t * l);
void free_index_list(index_list_t * l);

node_t * create_suffix_tree(char *s);
void add_suffix_suffix_tree(node_t *t, char *s, int start_index);
void print_suffixes(node_t *t);
void free_suffix_tree(node_t *t);

edge_list_t * find_edge(edge_list_t *edge_list, char c);
edge_list_t * insert_edge(edge_list_t *edge_list, char c);
void free_edge_list(edge_list_t * edges);


#endif
