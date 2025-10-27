#include <ncurses.h>
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define KEY_Q 113
#define KEY_J 106
#define KEY_K 107
#define KEY_L 108
#define KEY_H 104
#define KEY_G 71
#define KEY_GG 103

typedef struct {
  char *name;
  unsigned char type;
} entry;

typedef struct {
  entry entries[1024];
  char cwd_path[512];
  int len;
  int selected;
} state;

DIR *cwd;
struct dirent *de;
int input = 0;

void init() {
  cwd = opendir(".");
  if (cwd == NULL) {
    printf("failed to open dir");
    exit(1);
  }

  initscr();
  cbreak();
  keypad(stdscr, TRUE);
  noecho();
  curs_set(0);
  start_color();
  use_default_colors();

  init_pair(1, COLOR_BLUE, -1);
  init_pair(2, COLOR_WHITE, -1);
  init_pair(3, COLOR_BLACK, COLOR_BLUE);
}

void cleanup() {
  closedir(cwd);
  endwin();
}

void getentries(state *s) {
  getcwd(s->cwd_path, 512);
  s->len = 0;
  s->selected = 0;

  while ((de = readdir(cwd)) != NULL) {
    if (!strcmp(de->d_name, "..") || !strcmp(de->d_name, ".")) {
      continue;
    }

    s->entries[s->len].name = de->d_name;
    s->entries[s->len].type = de->d_type;
    s->len++;
  }
}

void drawentries(state *s) {
  clear();

  attron(A_BOLD);
  printw("%s\n\n", s->cwd_path);
  attroff(A_BOLD);

  int color = 0;
  for (int i = 0; i < s->len; i++) {
    if (i == s->selected) {
      color = 3;
    } else if (s->entries[i].type == DT_DIR) {
      color = 1;
    } else {
      color = 2;
    }

    attron(COLOR_PAIR(color));
    if (s->entries[i].type == DT_DIR) {
      printw("%s/\n", s->entries[i].name);
    } else {
      printw("%s\n", s->entries[i].name);
    }
    attroff(COLOR_PAIR(color));
  }

  refresh();
}

void changedir(state *s, const char *name) {
  closedir(cwd);
  chdir(name);
  cwd = opendir(".");
  getentries(s);
  drawentries(s);
}

void openfile(char *name) {
  char *editor = getenv("EDITOR");
  char *argv[] = {editor, name, NULL};
  execvp(editor, argv);
}

int main() {
  state s = {0};
  init();

  getentries(&s);
  drawentries(&s);

  while (input != KEY_Q) {
    if (input == KEY_J && s.selected < s.len - 1) {
      s.selected++;
      drawentries(&s);
    }
    if (input == KEY_K && s.selected > 0) {
      s.selected--;
      drawentries(&s);
    }
    if (input == KEY_GG) {
      s.selected = 0;
      drawentries(&s);
    }
    if (input == KEY_G) {
      s.selected = s.len - 1;
      drawentries(&s);
    }
    if (input == '\n' || input == KEY_ENTER || input == KEY_L) {
      entry selected = s.entries[s.selected];
      if (selected.type == DT_DIR) {
        changedir(&s, selected.name);
      } else {
        openfile(selected.name);
      }
    }
    if (input == KEY_H) {
      changedir(&s, "..");
    }

    input = getch();
  }

  cleanup();

  return 0;
}
