#ifndef SUFFIX_OPS_H_INCLUDED
#define SUFFIX_OPS_H_INCLUDED


/* operations on a suffix tree
 */
int has_substring(node_t *t, char *s);
index_list_t * get_substring_indices(node_t *t, char *s);


#endif
