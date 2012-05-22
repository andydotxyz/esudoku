// TODO - add some iteration stuff

// debug stuff

#define DEBUG(level, x, y, f, ...) if (level <= debug) { \
  char *__cell = grid_ref(x, y); \
  printf(f, __cell, __VA_ARGS__); \
  free(__cell); \
}

#define DEBUG_SIMPLE(level, f, ...) if (level <= debug) { \
  printf(f, __VA_ARGS__); \
}

#define ENTER(name) if (debug >= 3) { \
  printf("Enter rule %s\n", name); \
}

#define EXIT(name, ret) if (debug >= 3) { \
  printf("Exit rule %s ", name); \
  if (ret) \
    printf("matching\n"); \
  else \
    printf("no matches\n"); \
}

