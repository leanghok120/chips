#include <ncurses.h>
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>

#define KEY_Q 113
#define KEY_J 106
#define KEY_K 107

typedef struct {
  char *name;
  unsigned char type;
} entry;

DIR *cwd;
struct dirent *de;
entry entries[1024];
int len = 0;
int selected = 0;
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
}

void cleanup() {
  closedir(cwd);
  endwin();
}

void getentries() {
  while ((de = readdir(cwd)) != NULL) {
    entries[len].name = de->d_name;
    entries[len].type = de->d_type;
    len++;
  }
}

void drawentries() {
  clear();
  for (int i = 0; i < len; i++) {
    if (i == selected) {
      attron(A_REVERSE);
    }

    if (entries[i].type == DT_DIR) {
      printw("%s/\n", entries[i].name);
    } else {
      printw("%s\n", entries[i].name);
    }

    attroff(A_REVERSE);
  }

  refresh();
}

int main() {
  init();

  getentries();
  drawentries();

  while (input != KEY_Q) {
    if (input == KEY_J && selected < len - 1) {
      selected++;
      drawentries();
    }
    if (input == KEY_K && selected > 0) {
      selected--;
      drawentries();
    }

    input = getch();
  }

  cleanup();

  return 0;
}
