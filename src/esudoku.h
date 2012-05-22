#ifndef ESUDOKU_H
#define ESUDOKU_H

#define GRID_SIZE 3
#define ROW_SIZE 9

#define RULE_SIMPLE 1
#define RULE_EASY 10
#define RULE_MEDIUM 100
#define RULE_HARD 1000
#define RULE_DIABOLICAL 10000

#define SCORE_SIMPLE 25
#define SCORE_EASY 150
#define SCORE_MEDIUM 900
#define SCORE_HARD 5400
#define SCORE_IMPOSSIBLE 99999

#define debug 1

typedef struct {
    int count;
      int possibilities[ROW_SIZE];
} Possible;

#include "core.h"
#include "rules.h"
#include "macros.h"
#include "util.h"

#include <stdlib.h>
#include <stdio.h>
#include <sys/time.h>
#include <time.h>
#include <string.h>

void update_possibilities(int x, int y);
void reset_grid(void);
void print_grid(void);
void print_poss_grid(void);

int main_grid[ROW_SIZE][ROW_SIZE];
Possible poss_grid[ROW_SIZE][ROW_SIZE];

#endif

