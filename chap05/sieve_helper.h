
void local_set_params(int startval, int endval, int rank, int size, 
		      int *low_value, int *high_value, int *set_size);

int num_odd_past(int m, int bnd);

void initialize_grid(char **grid, int len);

void fill_grid_v1(char *grid_local, int rootn, int local_low, int local_setsize, int rank);

void fill_grid_rootn(char *grid_rootn, int rootn, int rootn_setsize);

void fill_grid_local_v2(char *grid_locol, char *grid_rootn, int rootn_setsize, 
			int local_low, int local_high, int local_setsize);

void fill_grid_local_v3(char *grid_local, char *grid_rootn, int rootn_setsize,
			int local_low, int local_high, int p);

void fill_grid_local_v4(char *grid_local, char *grid_rootn, int rootn_setsize,  
			int local_low, int local_setsize, int rank, int size);

int count_primes(char *grid, int len);
