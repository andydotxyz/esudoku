#include "esudoku.h"

int rule_last_possibility(int *score) {
  int x, y, z;
  int ret = 0;

  ENTER("last_possibility");
  for (y = 0; y < ROW_SIZE; y++) {
    for (x = 0; x < ROW_SIZE; x++) {
      if (poss_grid[x][y].count == 1) {
        for (z = 0; z < ROW_SIZE; z++) {
          if (poss_grid[x][y].possibilities[z]) {
            main_grid[x][y] = z + 1;
            poss_grid[x][y].possibilities[z] = 0;
            poss_grid[x][y].count = 0;

            DEBUG(1, x, y, "%s: %d is the last possibility\n", z + 1);
            *score += RULE_SIMPLE;
            ret = 1;
            update_possibilities(x, y);
            break;
          }
        }
      }
    }
  }

  EXIT("last_possibility", ret);
  return ret;
}

int rule_only_possible_place_in_row(int *score) {
  int x, y, z, a;
  int ret = 0;

  ENTER("only_possible_place_in_row");
  for (z = 0; z < ROW_SIZE; z++) {
    for (y = 0; y < ROW_SIZE; y++) {
      int found = 0;
      int poss_count = 0;
      int last_place;

      for (x = 0; x < ROW_SIZE; x++) {
        if (main_grid[x][y] == z + 1) {
          found = 1;
          break;
        } else if (poss_grid[x][y].possibilities[z]) {
          poss_count++;
          last_place = x;
        }
      }
        
      if (!found && poss_count == 1) {
        main_grid[last_place][y] = z + 1;
        for (a = 0; a < ROW_SIZE; a++)
          poss_grid[last_place][y].possibilities[a] = 0;
        poss_grid[last_place][y].count = 0;
        DEBUG(1, last_place, y, "%s: must be %d, it is the only possible place in the row\n", z + 1);
        *score += RULE_EASY;
        ret = 1;
        update_possibilities(last_place, y);
      }
    }
  }

  EXIT("only_possible_place_in_row", ret);
  return ret;
}

int rule_only_possible_place_in_col(int *score) {
  int x, y, z, a;
  int ret = 0;

  ENTER("only_possible_place_in_col");
  for (z = 0; z < ROW_SIZE; z++) {
    for (x = 0; x < ROW_SIZE; x++) {
      int found = 0;
      int poss_count = 0;
      int last_place;

      for (y = 0; y < ROW_SIZE; y++) {
        if (main_grid[x][y] == z + 1) {
          found = 1;
          break;
        } else if (poss_grid[x][y].possibilities[z]) {
          poss_count++;
          last_place = y;
        }
      }
        
      if (!found && poss_count == 1) {
        main_grid[x][last_place] = z + 1;
        for (a = 0; a < ROW_SIZE; a++)
          poss_grid[x][last_place].possibilities[a] = 0;
        poss_grid[x][last_place].count = 0;
        DEBUG(1, x, last_place, "%s: must be %d, it is the only possible place in the column\n", z + 1);
        *score += RULE_EASY;
        ret = 1;
        update_possibilities(x, last_place);
      }
    }
  }

  EXIT("only_possible_place_in_col", ret);
  return ret;
}

int rule_only_possible_place_in_box(int *score) {
  int x, y, xx, yy, z, a;
  int ret = 0;

  ENTER("only_possible_place_in_box");
  for (z = 0; z < ROW_SIZE; z++) {
    for (y = 0; y < ROW_SIZE; y += GRID_SIZE) {
      for (x = 0; x < ROW_SIZE; x += GRID_SIZE) {
        int found = 0;
        int poss_count = 0;
        int last_place_x, last_place_y;

        for (xx = x; xx < x + GRID_SIZE; xx++) {
          for (yy = y; yy < y + GRID_SIZE; yy++) {
            if (main_grid[xx][yy] == z + 1) {
              found = 1;
              break;
            } else if (poss_grid[xx][yy].possibilities[z]) {
              poss_count++;
              last_place_x = xx;
              last_place_y = yy;
            }
          }
        }
        
        if (!found && poss_count == 1) {
          main_grid[last_place_x][last_place_y] = z + 1;
          for (a = 0; a < ROW_SIZE; a++)
            poss_grid[last_place_x][last_place_y].possibilities[a] = 0;
          poss_grid[last_place_x][last_place_y].count = 0;
          DEBUG(1, last_place_x, last_place_y, "%s: must be %d, it is the only possible place in the box\n", z + 1);
          *score += RULE_EASY;
          ret = 1;
          update_possibilities(last_place_x, last_place_y);
        }
      }
    }
  }

  EXIT("only_possible_place_in_box", ret);
  return ret;
}

int rule_must_be_in_certain_box(int *score) {
  int x, y, xx, yy, z;
  int ret = 0;

  ENTER("must_be_in_certain_box");
  for (z = 0; z < ROW_SIZE; z++) {
    for (y = 0; y < ROW_SIZE; y += GRID_SIZE) {
      for (x = 0; x < ROW_SIZE; x += GRID_SIZE) {
        int row = -1;
        int col = -1;
        int breaking_row = 0;
        int breaking_col = 0;

        for (xx = x; xx < x + GRID_SIZE && (!breaking_row || !breaking_col); xx++) {
          for (yy = y; yy < y + GRID_SIZE && (!breaking_row || !breaking_col); yy++) {
            if (poss_grid[xx][yy].possibilities[z]) {
              if (row == -1)
                row = yy;
              else if (row != yy)
                breaking_row = 1;

              if (col == -1)
                col = xx;
              else if (col != xx)
                breaking_col = 1;
            }
          }
        }

        if (!breaking_row && row != -1) {
          for (xx = 0; xx < ROW_SIZE; xx++) {
            if (xx < x || xx >= x + GRID_SIZE) {
              /* if not in that box, remove */
              if (poss_grid[xx][row].possibilities[z]) {
                DEBUG(1, xx, row, "%s: cannot be %d, as it must appear elsewhere in the row\n", z + 1);
                *score += RULE_MEDIUM;
                poss_grid[xx][row].possibilities[z] = 0;
                poss_grid[xx][row].count--;
                ret = 1;
              }
            }
          }
        }
        if (!breaking_col && col != -1) {
          for (yy = 0; yy < ROW_SIZE; yy++) {
            if (yy < y || yy >= y + GRID_SIZE) {
              /* if not in that box, remove */
              if (poss_grid[col][yy].possibilities[z]) {
                DEBUG(1, col, yy, "%s: cannot be %d, as it must appear elsewhere in the column\n", z + 1);
                *score += RULE_MEDIUM;
                poss_grid[col][yy].possibilities[z] = 0;
                poss_grid[col][yy].count--;
                ret = 1;
              }
            }
          }
        }

      }
    }
  }

  EXIT("must_be_in_certain_box", ret);
  return ret;
}

int rule_eliminate_subset_extras_slave(Possible set, int x, int y, int *score) {
  int ii;
  int ret = 0;

  if (set.count <= 2)
    return 0;

  for (ii = 0; ii < ROW_SIZE; ii++) {
    if (set.possibilities[ii]) {
      int xx, yy, startx, starty;
      int matches = 0;
      char *subsetstr, *ptr;

      Possible subset = possible_clone(set); \
      subset.possibilities[ii] = 0; \
      subset.count--;
      subsetstr = malloc((subset.count + 1) * sizeof(char));
      ptr = subsetstr;
      xx = 0;
      for (xx = 0; xx < ROW_SIZE; xx++) {
        if (subset.possibilities[xx]) {
          *ptr = (char) xx + 1 + 48;
          ptr++;
        }
      }
      *ptr = '\0';

      ret = rule_eliminate_subset_extras_slave(subset, x, y, score) || ret;

      for (xx = 0; xx < ROW_SIZE; xx++) {
        if (possible_intersects(subset, poss_grid[xx][y]))
          matches++;
      }
      if (matches == subset.count) {
        for (xx = 0; xx < ROW_SIZE; xx++) {
          if (subset.count < poss_grid[xx][y].count &&
              possible_intersects(subset, poss_grid[xx][y])) {
            possible_intersect(&poss_grid[xx][y], subset);
            DEBUG(1, xx, y, "%s: subset {%s} cycle on row, eliminate extras\n", subsetstr);
            *score += RULE_HARD;
          }
        }
        ret = 1;
      }

      matches = 0;
      for (yy = 0; yy < ROW_SIZE; yy++) {
        if (possible_intersects(subset, poss_grid[x][yy]))
          matches++;
      }
      if (matches == subset.count) {
        for (yy = 0; yy < ROW_SIZE; yy++) {
          if (subset.count < poss_grid[x][yy].count &&
              possible_intersects(subset, poss_grid[x][yy])) {
            possible_intersect(&poss_grid[x][yy], subset);
            DEBUG(1, x, yy, "%s: subset {%s} cycle on col, eliminate extras\n", subsetstr);
            *score += RULE_HARD;
          }
        }
        ret = 1;
      }

      matches = 0;
      startx = x - (x % GRID_SIZE);
      starty = y - (y % GRID_SIZE);
      for (xx = startx; xx < startx + GRID_SIZE; xx++) {
        for (yy = starty; yy < starty + GRID_SIZE; yy++) {
          if (possible_intersects(subset, poss_grid[xx][yy]))
            matches++;
        }
      }
      if (matches == subset.count) {
        for (xx = startx; xx < startx + GRID_SIZE; xx++) {
          for (yy = starty; yy < starty + GRID_SIZE; yy++) {
            if (subset.count < poss_grid[xx][yy].count &&
                possible_intersects(subset, poss_grid[xx][yy])) {
              possible_intersect(&poss_grid[xx][yy], subset);
              DEBUG(1, xx, yy, "%s: subset {%s} cycle on box, eliminate extras\n", subsetstr);
              *score += RULE_HARD;
            }
          }
        }
        ret = 1;
      }

      free(subsetstr);
    }
  }
  return ret;
}

int rule_eliminate_subset_extras(int *score) {
  int x, y;
  int ret = 0;

  ENTER("eliminate_subset_extras");

  for (y = 0; y < ROW_SIZE; y++) {
    for (x = 0; x < ROW_SIZE; x++) {

      if (poss_grid[x][y].count > 1) {
        /* if it is not the largest possible set in the area */
        ret = rule_eliminate_subset_extras_slave(poss_grid[x][y], x, y, score)
            || ret;
      }

    }
  }
  EXIT("eliminate_subset_extras", ret);
  return ret;
}

int rule_xwing_row(int *score) {
  int x, y, z;
  int ret = 0;

  ENTER("xwing_row");

  for (z = 0; z < ROW_SIZE; z++) {
    int found_y1 = 0, found_y2 = 0;
    int y_count = 0;
    int found_x1, found_x2;

    for (y = 0; y < ROW_SIZE; y++) {
      int x_count = 0, x1 = 0, x2 = 0;

      for (x = 0; x < ROW_SIZE; x++) {

        if (poss_grid[x][y].possibilities[z]) {
          x_count++;

          if (y_count) {
            if (x != found_x1 && x != found_x2) {
              x_count = 0;
              break;
            }
          } else {

            if (x1)
              x2 = x;
            else
              x1 = x;
          }
        }

      }

      if (x_count == 2) {
        if (!y_count) {
          found_x1 = x1;
          found_x2 = x2;
        }
        y_count++;

        if (y_count > 2) {
          y_count = 0;
          break;
        }

        if (found_y1)
          found_y2 = y;
        else
          found_y1 = y;
      }
    }

    if (y_count == 2) {
      int yy;

      for (yy = 0; yy < ROW_SIZE; yy++) {
        if (yy == found_y1 || yy == found_y2)
          continue;

        if (poss_grid[found_x1][yy].possibilities[z]) {
          poss_grid[found_x1][yy].possibilities[z] = 0;
          poss_grid[found_x1][yy].count--;
          ret = 1;
        }

        if (poss_grid[found_x2][yy].possibilities[z]) {
          poss_grid[found_x2][yy].possibilities[z] = 0;
          poss_grid[found_x2][yy].count--;
          ret = 1;
        }
      }

      if (ret) {
        char *grid = grid_ref(found_x2, found_y2);
        DEBUG(1, found_x1, found_y1, "XWING rows found with diagonals %s and %s - eliminating '%d' on cols\n", grid, z + 1)
        free(grid);

        *score += RULE_DIABOLICAL;
      }
    }
  }

  EXIT("xwing_row", ret);
  return ret;
}

int rule_xwing_col(int *score) {
  int x, y, z;
  int ret = 0;

  ENTER("xwing_col");

  for (z = 0; z < ROW_SIZE; z++) {
    int found_x1 = 0, found_x2 = 0;
    int x_count = 0;
    int found_y1, found_y2;

    for (x = 0; x < ROW_SIZE; x++) {
      int y_count = 0, y1 = 0, y2 = 0;

      for (y = 0; y < ROW_SIZE; y++) {

        if (poss_grid[x][y].possibilities[z]) {
          y_count++;

          if (x_count) {
            if (y != found_y1 && y != found_y2) {
              y_count = 0;
              break;
            }
          } else {

            if (y1)
              y2 = y;
            else
              y1 = y;
          }
        }

      }

      if (y_count == 2) {
        if (!x_count) {
          found_y1 = y1;
          found_y2 = y2;
        }
        x_count++;

        if (x_count > 2) {
          x_count = 0;
          break;
        }

        if (found_x1)
          found_x2 = x;
        else
          found_x1 = x;
      }
    }

    if (x_count == 2) {
      int xx;

      for (xx = 0; xx < ROW_SIZE; xx++) {
        if (xx == found_x1 || xx == found_x2)
          continue;

        if (poss_grid[xx][found_y1].possibilities[z]) {
          poss_grid[xx][found_y1].possibilities[z] = 0;
          poss_grid[xx][found_y1].count--;
          ret = 1;
        }

        if (poss_grid[xx][found_y2].possibilities[z]) {
          poss_grid[xx][found_y2].possibilities[z] = 0;
          poss_grid[xx][found_y2].count--;

          ret = 1;
        }
      }

      if (ret) {
        char *grid = grid_ref(found_x2, found_y2);
        DEBUG(1, found_x1, found_y1, "XWING cols found with diagonals %s and %s - eliminating '%d' on rows\n", grid, z + 1)
        free(grid);

        *score += RULE_DIABOLICAL;
      }
    }
  }

  EXIT("xwing_col", ret);
  return ret;
}

