#ifndef GEN_SUFFIX_TREE_H_INCLUDED
#define GEN_SUFFIX_TREE_H_INCLUDED


/* linked list of edges with char and connecting child nodes */
struct gen_edge_list {
  char c;
  struct gen_node *child;
  struct gen_edge_list * next;
};
typedef struct gen_edge_list gen_edge_list_t;

/* linked list of index and string id */
struct gen_index_list {
  int string_id;
  int index;
  struct gen_index_list * next;
};
typedef struct gen_index_list gen_index_list_t;

/* node in suffix tree */
struct gen_node {
  gen_edge_list_t *edges;
  gen_index_list_t *indices;
};
typedef struct gen_node gen_node_t;


gen_index_list_t * insert_in_gen_index_list(gen_index_list_t *l, int string_id, int index);
void print_gen_index_list(gen_index_list_t * l);
void free_gen_index_list(gen_index_list_t * l);

gen_node_t * create_gen_suffix_tree();
void add_string_gen_suffix_tree(gen_node_t *t, char *s, int string_id);
void add_suffix_gen_suffix_tree(gen_node_t *t, char *s, int string_id, int start_index);
void free_gen_suffix_tree(gen_node_t *t);

gen_edge_list_t * find_gen_edge(gen_edge_list_t *edge_list, char c);
gen_edge_list_t * insert_gen_edge(gen_edge_list_t *edge_list, char c);
void free_gen_edge_list(gen_edge_list_t * edges);


#endif
