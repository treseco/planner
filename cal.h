#ifndef CAL_H
#define CAL_H

#include <string>
#include <vector>
#include "datetime.h"

class Calendar {

private:
  std::vector<Event> events;

public:
  Calendar();
  void load_events(std::string path);
  void save_events(std::string path);
  void print_month();
  void new_event();
  void remove_event();
};

#endif