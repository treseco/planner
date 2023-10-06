#ifndef CAL_H
#define CAL_H

#include <string>
#include <vector>
#include "datetime.h"

class Calendar {

private:
  CalendarRange range;
  std::vector<Event> events;

public:
  Calendar();
  void load_events(std::string path);
  void save_events(std::string path);
  void set_range(int by, unsigned bm, unsigned bd, int ey, unsigned em, unsigned ed);
  void print();
  void new_event();
  void remove_event();
};

#endif