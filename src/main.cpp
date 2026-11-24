#include "eadkpp.h"
#include "game.hpp"

extern const char eadk_app_name[] __attribute__((section(".rodata.eadk_app_name"))) = "IO Arcade";
extern const uint32_t eadk_api_level __attribute__((section(".rodata.eadk_api_level"))) = 1;

int main(int argc, char * argv[]) {
  (void)argc;
  (void)argv;
  Game game;
  game.run();
  return 0;
}
