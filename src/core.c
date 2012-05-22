#include "esudoku.h"

void update_possibilities(int x, int y) {
  int zx, zy;
  char *cell;
  int a = main_grid[x][y];
  
  cell = grid_ref(x, y);
  if (a != -1) {
    /* delete the possibility from the rest of the row... */
    for (zx = 0; zx < ROW_SIZE; zx++) {
      if (zx == x)
        continue;

      if (poss_grid[zx][y].possibilities[a - 1]) {
        poss_grid[zx][y].possibilities[a - 1] = 0;
        poss_grid[zx][y].count--;
        DEBUG(2, zx, y, "%s: %d is not valid as it already appears in the row at %s\n", a, cell);
      }
    }

    /* delete the possibility from the rest of the col... */
    for (zy = 0; zy < ROW_SIZE; zy++) {
      if (zy == y)
        continue;

      if (poss_grid[x][zy].possibilities[a - 1]) {
        poss_grid[x][zy].possibilities[a - 1] = 0;
        poss_grid[x][zy].count--;
        DEBUG(2, x, zy, "%s: %d is not valid as it already appears in the column at %s\n", a, cell);
      }
    }

    /* delete the possibility from the rest of the box... */
    {
      int sx = x - x % GRID_SIZE;
      int sy = y - y % GRID_SIZE;

      for (zy = sy; zy < sy + GRID_SIZE; zy++) {
        for (zx = sx; zx < sx + GRID_SIZE; zx++) {
          if (zx == x && zy == y)
            continue;

          if (poss_grid[zx][zy].possibilities[a - 1]) {
            poss_grid[zx][zy].possibilities[a - 1] = 0;
            poss_grid[zx][zy].count--;
            DEBUG(2, zx, zy, "%s: %d is not valid as it already appears in the box at %s\n", a, cell);
          }
        }
      }
    }
  }

  free(cell);
}


















int solve(int *score) {
  int x, y;
  int made_move = 1;
  int solved = 1;
  int broken = 0;

  /* catch folk who might try and solve an empty grid, it just wastes time */
  for (y = 0; y < ROW_SIZE; y++) {
    for (x = 0; x < ROW_SIZE; x++) {
      if (main_grid[x][y] != -1) {
        solved = 0;
        break;
      }
    }

    if (!solved)
      break;
  }
  if (solved) {
    printf("Empty grids cannot be solved!!!\n");
    *score = SCORE_IMPOSSIBLE;
    return 0;
  }

  solved = 1;
  load_poss_grid();
  while (made_move) {
    made_move = rule_last_possibility(score) ||
      rule_only_possible_place_in_row(score) ||
      rule_only_possible_place_in_col(score) ||
      rule_only_possible_place_in_box(score) ||
      rule_must_be_in_certain_box(score) ||
      rule_eliminate_subset_extras(score) ||
      rule_xwing_row(score) ||
      rule_xwing_col(score);
  }

  for (y = 0; y < ROW_SIZE; y++) {
    for (x = 0; x < ROW_SIZE; x++) {
      if (main_grid[x][y] == -1) {
        solved = 0;
        if (poss_grid[x][y].count == 0)
          broken = 1;
      }
    }
  }

  if (solved) {
    printf("YAY - we SOLVED that one :)\n");
    return 1;
  } else if (broken) {
    printf("BROKEN - this puzzle is not possible\n");
    return 0;
  } else {
    printf("Sorry Jim, I don't quite know how to do the rest...\n");
    *score = SCORE_IMPOSSIBLE;
    print_poss_grid();
    return 0;
  }
}

int generate_solution(void) {
  int x, y, z, size;

  ENTER("generate_solution");
  reset_grid();

  srandom((unsigned int) get_time());
  for (y = 0; y < ROW_SIZE; y++) {
    for (x = 0; x < ROW_SIZE; x++) {
      size = poss_grid[x][y].count;
      if (size == 0) {
        EXIT("generate_solution", 0);
        return 0;
      }
      z = (random() % ROW_SIZE);
      while (!poss_grid[x][y].possibilities[z]) {
        z = (random() % ROW_SIZE);
      }

      main_grid[x][y] = z + 1;
      poss_grid[x][y].possibilities[z] = 0;
      poss_grid[x][y].count--;
      update_possibilities(x, y);
    }
  }
  EXIT("generate_solution", 1);
  return 1;
}

int generate(int target) {
  int ret[ROW_SIZE][ROW_SIZE], tmp[ROW_SIZE][ROW_SIZE];
  int score, retscore;
  int x, y, z, bx, by;
  int passes, broken, tries, multiples;

  ENTER("generate");

  passes = 0;
  multiples = 0;
  while (!passes) {
    tries = 0;
    z = 1;
    while (!generate_solution()) {
      z++;
    }
    DEBUG_SIMPLE(1, "%d attempts to get a solution\n", z);

    retscore = 0;

    /* generate was OK, now eliminate */
    memcpy(&ret, &main_grid, sizeof(ret));
    broken = 0;
    while (!passes && tries < 25) {
      /* reset for another pass */
      memcpy(&main_grid, &ret, sizeof(ret));
      print_grid();
      load_poss_grid();
      tries++;

      /* precull some cells */
      for (y = 0; y < ROW_SIZE; y++) {
        for (x = 0; x < ROW_SIZE; x++) {
	  if ((random() % 3) == 0) {

            main_grid[x][y] = -1;
            for (z = 0; z < ROW_SIZE; z++)
              poss_grid[x][y].possibilities[z] = 1;
            poss_grid[x][y].count = ROW_SIZE;
          }
        }
      }

      while (retscore < target) {
        x = random() % ROW_SIZE;
        y = random() % ROW_SIZE;

        if (main_grid[x][y] == -1) {
          continue;
        }

        score = 0;
        main_grid[x][y] = -1;
        for (z = 0; z < ROW_SIZE; z++)
          poss_grid[x][y].possibilities[z] = 1;
        poss_grid[x][y].count = ROW_SIZE;
        memcpy(&tmp, &main_grid, sizeof(ret));

        if (!solve(&score)) {
          DEBUG_SIMPLE(1, "UNSOLVABLE - rolling back\n", NULL);
          broken = 1;
          break;
        }
        if (score >= target) {
          DEBUG_SIMPLE(1, "Too complex - rolling back\n", NULL);
          passes = 1;
          break;
        }
        /* roll back to before solve so we can aggregate eliminations */
        memcpy(&main_grid, &tmp, sizeof(ret));
        retscore = score;
      }

      if (!passes && !broken) {
        DEBUG_SIMPLE(1, "Generated puzzle not complex enough (%s), trying another\n", difficulty(retscore));
      }
    }
    if (!passes) {
      DEBUG_SIMPLE(1, "Tried too many times, generating a new soluition\n", NULL);
      multiples++;
    }
  }
  memcpy(&main_grid, &tmp, sizeof(ret));

  printf("Generated grid with difficulty %d (%s) after %d attempts.\n", retscore, difficulty(retscore), tries + (multiples * 25));
  EXIT("generate", 1);
  return 1;
}

int load_grid(char *numbers) {
  int x, y;
  char *n;

  n = numbers;
  for (y = 0; y < ROW_SIZE; y++) {
    for (x = 0; x < ROW_SIZE; x++) {
      if (!n || !*n) {
        fprintf(stderr, "Not enough numbers in the list\n");
        return 0;
      }

      if (*n == '-')
        main_grid[x][y] = -1;
      else
        main_grid[x][y] = (int) *n - 48;

      n++;
    }
  }
  if (n && *n) {
    fprintf(stderr, "Too many numbers in the list\n");
    return 0;
  }

  return 1;
}

void load_poss_grid(void) {
  int x, y, z;

  for (y = 0; y < ROW_SIZE; y++) {
    for (x = 0; x < ROW_SIZE; x++) {
      if (main_grid[x][y] == -1) {
        poss_grid[x][y].count = ROW_SIZE;
        for (z = 0; z < ROW_SIZE; z++) {
          poss_grid[x][y].possibilities[z] = 1;
        }

      } else {
        poss_grid[x][y].count = 0;
        for (z = 0; z < ROW_SIZE; z++) {
          poss_grid[x][y].possibilities[z] = 0;
        }
      }
    }
  }

  for (y = 0; y < ROW_SIZE; y++) {
    for (x = 0; x < ROW_SIZE; x++) {
      update_possibilities(x, y);
    }
  }
}

void reset_grid() {
  int x, y;
          
  for (y = 0; y < ROW_SIZE; y++) {
    for (x = 0; x < ROW_SIZE; x++) {
      main_grid[x][y] = -1;
    } 
  }     
  load_poss_grid();
}

void print_grid(void) {
  int y, x;

  for (y = 0; y < ROW_SIZE; y++) {
    if (y > 0 && (y % GRID_SIZE) == 0) {
      for (x = 0; x < ROW_SIZE + GRID_SIZE - 1; x++)
        printf("-");
      printf("\n");
    }

    for (x = 0; x < ROW_SIZE; x++) {
      if (x > 0 && (x % GRID_SIZE) == 0) 
        printf("|");

      if (main_grid[x][y] == -1)
        printf(" ");
      else
        printf("%d", main_grid[x][y]);
    }
    printf("\n");
  }
}

void print_poss_grid(void) {
  int y, x, printed;
  int size = 0;

  for (y = 0; y < ROW_SIZE; y++) {
    for (x = 0; x < ROW_SIZE; x++) {
      if (poss_grid[x][y].count > size)
        size = poss_grid[x][y].count;
    }
  }

  for (y = 0; y < ROW_SIZE; y++) {
    if (y > 0 && (y % GRID_SIZE) == 0) {
      for (x = 0; x < (ROW_SIZE * (size + 1)) + GRID_SIZE - 2; x++)
        printf("-");
      printf("\n");
    }

    for (x = 0; x < ROW_SIZE; x++) {
      if (x > 0 && (x % GRID_SIZE) == 0) 
        printf("|");

      printed = print_possible(poss_grid[x][y]);
      while (printed++ < size)
        printf(" ");
      printf(" ");
    }
    printf("\n");
  }
}

