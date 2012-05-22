#include "esudoku.h"
#include "config.h"

#ifdef BUILD_ETK
#include "gui_etk.h"
#endif

static void usage() {
  printf("Usage: esudoku command [command-opts]\n");
#ifdef BUILD_ETK
  printf("  where command is generate, solve or etk\n");
#else
  printf("  where command is generate or solve\n");
#endif
  printf("    generate takes no opts and generates a sudoku puzzle\n");
  printf("    solve takes 1 parameter, the list of numbers to make a puzzle and solves is\n");
#ifdef BUILD_ETK
  printf("    etk takes no parameters and shows an ETK build GUI interface\n");
#endif
}

int main(int argc, char **argv) {
  long start, end, diff;

#ifdef BUILD_ETK
  if (argc == 1) {
    _gui_etk_load(&argc, &argv);
    return 0;
  } else if (argc < 1) {
    usage();
    return 1;
  }
#endif
#ifndef BUILD_ETK
  if (argc < 2) {
    usage();
    return 1;
  }
#endif

  if (!strcmp(argv[1], "generate")) {
    int score = SCORE_MEDIUM;
    if (argc >= 3) {
      score = get_score(argv[2]);
      if (score == -1) {
        printf("invalid score, please try simple, easy, moderate, hard or diabolical\n");
        printf("using moderate");
        score = SCORE_MEDIUM;
      }
    }

    generate(score);
    print_grid();
  } else if (!strcmp(argv[1], "help")) {
    usage();
    return 0;

#ifdef BUILD_ETK
  } else if(!strcmp(argv[1], "etk")) {
    _gui_etk_load(&argc, &argv);
#endif
  } else if (!strcmp(argv[1], "solve")) {
    int score = 0;    
    if (argc < 3) {
      printf("solve command needs an extra option, the grid to solve (e.g. 12-3-4 etc)\n");
      return 1;
    }

    if (!load_grid(argv[2]))
      return 1;
    print_grid();

    start = get_time();
    solve(&score);
    end = get_time();
    diff = end - start;

    printf("finished in ");
    if (diff < 1000)
      printf("%ld microseconds\n", diff);
    else {
      diff /= 1000;
      if (diff < 1000)
        printf("%ld milliseconds\n", diff);
      else
        printf("%ld seconds\n", diff / 1000);
    }
    printf("Difficulty score was %d (%s).\n", score, difficulty(score));

    print_grid();
  
  } else {
    printf("unrecognised command, %s\n", argv[1]);
    usage();
    return 1;
  }

  return 0;
}

