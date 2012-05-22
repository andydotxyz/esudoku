int rule_last_possibility(int *score);
int rule_only_possible_place_in_row(int *score);
int rule_only_possible_place_in_col(int *score);
int rule_only_possible_place_in_box(int *score);

int rule_must_be_in_certain_box(int *score);
int rule_eliminate_subset_extras(int *score);

int rule_xwing_row(int *score);
int rule_xwing_col(int *score);
