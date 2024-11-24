#include "console_app.h"
#include "gui_app.h"
#include <string.h>

#define STRING_EQUAL 0

static const char console_game_argument[] = "--console";

int main(int argc, char *argv[]) {
  if (argc == 2 && 
      strncmp(argv[1], console_game_argument, sizeof(console_game_argument)) == STRING_EQUAL) {
    return console_main();
  }

  return gui_main();
}
