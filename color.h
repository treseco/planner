#ifndef COLOR_H
#define COLOR_H

#include <string>

#define RESET "\x1b[0m"
#define BLACK "\x1b[30m"
#define BGBLACK "\x1b[40m"
#define RED "\x1b[31m"
#define BGRED "\x1b[41m"
#define WHITE "\x1b[37m"
#define BGWHITE "\x1b[47m"
#define GREEN "\x1b[32m"
#define BGGREEN "\x1b[42m"
#define MAGENTA "\x1b[35m"
#define BGMAGENTA "\x1b[45m"
#define YELLOW "\x1b[33m"
#define BGYELLOW "\x1b[43m"
#define CYAN "\x1b[36m"
#define BGCYAN "\x1b[46m"
#define BLUE "\x1b[34m"
#define BGBLUE "\x1b[44m"
#define GRAY "\x1b[90m"
#define BGGRAY "\x1b[100m"

#define NUM_COLORS 6

extern std::string fg_colors[NUM_COLORS];

extern std::string bg_colors[NUM_COLORS];

std::string color(std::string s, std::string c);

#endif
