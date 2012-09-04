


/* Start a client to query string */
result_list_t *query_string(char *str);

/* Recieve query_string_success signal */
result_list_t *recv_query_result(int sock_addr);

/* Create new result list */
result_list_t *create_result_list();

/* Add new val to result list */
void add_result(result_list_t *rl, int val);

/* Print result list */
void print_result_list(result_list_t *rl);
