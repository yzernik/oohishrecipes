#ifndef GEN_SUFFIX_OPS_H_INCLUDED
#define GEN_SUFFIX_OPS_H_INCLUDED


/* operations on a generalized suffix tree
 */
int has_substring(gen_node_t *t, char *s);
gen_index_list_t * get_substring_indices(gen_node_t *t, char *s);


#endif
