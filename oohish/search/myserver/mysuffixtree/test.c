/* Suffix Tree program */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mygensuffixtree.h"
#include "genoperations.h"


int main()
{
  //create generalized suffix tree
  gen_node_t *my_gen_tree;
  my_gen_tree = create_gen_suffix_tree();
  add_string_gen_suffix_tree(my_gen_tree, "green onions", 1);
  add_string_gen_suffix_tree(my_gen_tree, "lentils", 2);

  //find all substring matches and print string id and index
  gen_index_list_t *leaf_pairs;
  leaf_pairs = get_substring_indices(my_gen_tree, "en");
  print_gen_index_list(leaf_pairs);

  //free memory
  free_gen_suffix_tree(my_gen_tree);
  
  return 0;
}
