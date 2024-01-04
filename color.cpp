#include <string>
#include "color.h"

std::string fg_colors[] = {
  BLUE, YELLOW, MAGENTA, GREEN, CYAN, RED,
};

std::string bg_colors[] = {
  BGBLUE, BGYELLOW, BGMAGENTA, BGGREEN, BGCYAN, BGRED,
};

std::string color(std::string s, std::string c) { 
  return c + s + RESET;
}


