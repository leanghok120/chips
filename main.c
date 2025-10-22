#include <ncurses.h>
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>

DIR *cwd;
struct dirent *de;

void init() {
  cwd = opendir(".");
  if (cwd == NULL) {
    printf("failed to open dir");
    exit(1);
  }

  initscr();
  raw();
  keypad(stdscr, TRUE);
  noecho();
}

void cleanup() {
  refresh();
  getch();
  closedir(cwd);
  endwin();
}

int main() {
  init();

  while ((de = readdir(cwd)) != NULL) {
    if (de->d_type == DT_DIR) {
      attron(A_REVERSE);
      printw("%s\n", de->d_name);
      attroff(A_REVERSE);
      continue;
    }
    printw("%s\n", de->d_name);
  }

  cleanup();

  return 0;
}
