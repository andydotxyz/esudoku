#include "esudoku.h"

#include <Etk.h>
#include <Enhance.h>

Enhance *en;
Etk_Entry *entry_grid[9][9];

void _gui_show_grid(void);
void _gui_load_grid(void);

void on_new_activated(Etk_Object *obj, void **data) {
  int x, y;

  for (y = 0; y < 9; y++) {
    for (x = 0; x < 9; x++) {
      etk_entry_text_set(entry_grid[x][y], "");
    }
  }
}
void on_generate_activated(Etk_Object *obj, void **data) {
  generate(SCORE_MEDIUM);
  _gui_show_grid();
}

void on_solve_activated(Etk_Object *obj, void **data) {
  int score;

  _gui_load_grid();
  solve(&score);
  _gui_show_grid();
}

void on_quit_activated(Etk_Object *obj, void **data) {
  etk_main_quit();
}

void on_about_activated(Etk_Object *obj, void **data) {
  etk_widget_show(enhance_var_get(en, "dialog1"));
}

int on_window1_delete_event(void *data) {
  etk_main_quit();
  return 1;
}

void _gui_etk_load(int *argc, char ***argv) {
  char *name;
  int x, y;

  enhance_init();
  etk_init(argc, argv);
  en = enhance_new();
  enhance_file_load(en, "window1", "src/esudoku.glade");

  name = malloc(9 * sizeof(char));
  for (y = 0; y < 9; y++) {
    for (x = 0; x < 9; x++) {
      snprintf(name, 9, "grid-%d-%d", y, x);
      entry_grid[x][y] = ETK_ENTRY(enhance_var_get(en, name));
    }
  }
  free(name);

  etk_main();

  etk_main_quit();
  etk_shutdown();
  enhance_free(en);
  enhance_shutdown();
}

void _gui_load_grid(void) {
  int x, y;
  const char *value;
  Etk_Entry *wid;

  for (y = 0; y < 9; y++) {
    for (x = 0; x < 9; x++) {
      value = etk_entry_text_get(entry_grid[x][y]);
      if (!value || strlen(value) == 0)
        main_grid[x][y] = -1;
      else
        main_grid[x][y] = atoi(value);
    }
  }
}

void _gui_show_grid(void) {
  int x, y;
  char *value;
  Etk_Entry *wid;
 
  value = malloc(2 * sizeof(char));
  for (y = 0; y < 9; y++) {
    for (x = 0; x < 9; x++) {
      if (main_grid[x][y] != -1) {
        snprintf(value, 2, "%d", main_grid[x][y]);
        etk_entry_text_set(entry_grid[x][y], value);
      } else {
        etk_entry_text_set(entry_grid[x][y], "");
      }
    }
  }
}
