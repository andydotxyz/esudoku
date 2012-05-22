int possible_is_sameset(Possible seta, Possible setb);
int possible_is_subset(Possible subset, Possible set);
int possible_intersects(Possible seta, Possible setb);
void possible_intersect(Possible *modify, Possible reference);
void possible_subtract(Possible *modify, Possible reference);
Possible possible_clone(Possible poss);

long get_time();
char *grid_ref(int x, int y);
char *difficulty(int score);
int  get_score(char *difficulty);
	
int print_possible(Possible in);
