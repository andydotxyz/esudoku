#include "esudoku.h"

int possible_is_sameset(Possible seta, Possible setb) {
  int i;

  if (seta.count != setb.count)
    return 0;

  for (i = 0; i < ROW_SIZE; i++) {
    if (seta.possibilities[i] != setb.possibilities[i])
      return 0;
  }

  return 1;
}

int possible_is_subset(Possible subset, Possible set) {
  int i;

  if (subset.count > set.count)
    return 0;

  for (i = 0; i < ROW_SIZE; i++) {
    if (subset.possibilities[i]) {
      if (!set.possibilities[i])
        return 0;
    }
  }
  return 1;
}

int possible_intersects(Possible seta, Possible setb) {
  int i;

   for (i = 0; i < ROW_SIZE; i++) {
     if (seta.possibilities[i]) {
       if (setb.possibilities[i]) {
         return 1;
       }
     }
   }
   return 0;
}

void possible_intersect(Possible *modify, Possible reference) {
  int i;

  for (i = 0; i < ROW_SIZE; i++) {
    if (!reference.possibilities[i]) {
      if ((*modify).possibilities[i]) {
        (*modify).possibilities[i] = 0;
        (*modify).count--;
      }
    }
  }
}

void possible_subtract(Possible *modify, Possible reference) {
  int i;

  for (i = 0; i < ROW_SIZE; i++) {
    if (reference.possibilities[i]) {
      if ((*modify).possibilities[i]) {
        (*modify).possibilities[i] = 0;
        (*modify).count--;
      }
    }
  }
}

Possible possible_clone(Possible poss) {
  Possible ret;

  memcpy((void *) &ret, (void *) &poss, sizeof(Possible));
  return ret;
}

long get_time() {
  struct timeval time;

  gettimeofday(&time, NULL);
  return time.tv_usec + time.tv_sec * 1000000;
}

char *grid_ref(int x, int y) {
  char *ret = malloc(sizeof(char) * 3);

  ret[0] = (char) x + 65;
  ret[1] = (char) y + 1 + 48;
  ret[2] = '\0';

  return ret;
}

char *difficulty(int score) {
  if (score <= SCORE_SIMPLE)
    return "simple";
  if (score <= SCORE_EASY)
    return "easy";
  if (score <= SCORE_MEDIUM)
    return "moderate";
  if (score <= SCORE_HARD)
    return "hard";

  if (score == SCORE_IMPOSSIBLE)
    return "impossible";
  return "diabolical";
}

int get_score(char *difficulty) {
  if (!strcmp(difficulty, "simple"))
    return SCORE_SIMPLE;
  if (!strcmp(difficulty, "easy"))
    return SCORE_EASY;
  if (!strcmp(difficulty, "moderate"))
    return SCORE_MEDIUM;
  if (!strcmp(difficulty, "hard"))
    return SCORE_HARD;
  if (!strcmp(difficulty, "diabolical"))
    return 99999;
  return -1;
}

int print_possible(Possible in) {
  int i;
  int printed = 0;

  for (i = 0; i < ROW_SIZE; i++) {
    if (in.possibilities[i]) {
      printf("%d", i + 1);
      printed++;
    }
  }

  return printed;
}
