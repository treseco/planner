#include <fstream>
#include <iostream>
#include <iomanip>
#include <unordered_map>

#include "cal.h"
#include "datetime.h"

//CalendarRange
Calendar::Calendar() {}

//TODO: this is a temporary solution. currently using format
//to make future integration with icalendar files easier.
//in this current form this is not guaranteed to work properly
//with ics files and does not properly check for errors.
void Calendar::load_events(std::string path) {
  std::string line;
  std::string key;
  std::string value;
  size_t pos;
  std::string title;
  std::string tag;
  Date begin;
  Date end;

  std::ifstream ifs;
  ifs.open(path, std::ifstream::in);

  std::getline(ifs, line);
  while(ifs.good()) {
    if(line[0] != ' ') {
      if(line.empty() || (pos = line.find(':')) == std::string::npos) break;
      key   = line.substr(0, pos);
      value = line.substr(pos+1, line.length()-pos-2);
      if(key == "SUMMARY") title = value;
      else if(key == "DESCRIPTION") tag = value;
      else if(key == "DTSTART") {
        int year = atoi(value.substr(0,4).c_str());
        unsigned month = static_cast<unsigned>(atoi(value.substr(4,2).c_str()));
        unsigned day = static_cast<unsigned>(atoi(value.substr(6,2).c_str()));
        begin = Date(year, month, day);
      } else if(key == "DTEND") {
        int year = atoi(value.substr(0,4).c_str());
        unsigned month = static_cast<unsigned>(atoi(value.substr(4,2).c_str()));
        unsigned day = static_cast<unsigned>(atoi(value.substr(6,2).c_str()));
        end = Date(year, month, day);
      } else if(key == "END" && value == "VEVENT") {
        events.emplace_back(Event(title, tag, begin, end));
      }
    }
    std::getline(ifs, line);
  }

  ifs.close();
}

//TODO: this is a temporary solution. currently using format
//to make future integration with icalendar files easier.
//proper error handling is also needed still.
void Calendar::save_events(std::string path) {
  std::ofstream ofs;
  ofs.open(path, std::ofstream::out);

  ofs << "BEGIN:VCALENDAR" << "\r\n";

  for(size_t i = 0; i < events.size(); i++) {
    ofs << "BEGIN:VEVENT" << "\r\n"
        << "SUMMARY:" << events[i].get_title() << "\r\n"
        << "DESCRIPTION:" << events[i].get_tag() << "\r\n"
        << "DTSTART:" << events[i].get_begin().to_tz_tstamp() << "\r\n"
        << "DTEND:" << events[i].get_end().to_tz_tstamp() << "\r\n"
        << "END:VEVENT" << "\r\n";
  }

  ofs << "END:VCALENDAR" << "\r\n";

  ofs.close();
}

void Calendar::set_range(int by, unsigned bm, unsigned bd, int ey, unsigned em, unsigned ed) {
  Date begin = Date(by, bm, bd);
  Date end   = Date(ey, em, ed);
  range = CalendarRange(begin, end);
}

void Calendar::print() {
  range.set_events(&events);

  //print out calendar with events
  std::cout << range.print_cal();
}

void Calendar::new_event() {
  std::string title;
  std::string tag;
  std::string begin;
  std::string end;

  //read in event details from cin
  std::cout << "Enter event title: ";
  std::getline(std::cin, title);
  std::cout << "Enter event tag (four character abreviation): ";
  std::cin >> tag;
  std::cout << "Enter event start date as MM/DD/YYYY: ";
  std::cin >> begin;
  std::cout << "Enter event end as MM/DD/YYYY: ";
  std::cin >> end;

  size_t del_idx;
  unsigned begin_m;
  unsigned begin_d;
  int begin_y;
  unsigned end_m;
  unsigned end_d;
  int end_y;

  //TODO: make a helper function and check for exceptions
  //parse input data
  del_idx = begin.find('/');
  begin_m = static_cast<unsigned>(std::stoi(begin.substr(0, del_idx)));
  begin.erase(0, del_idx + 1);
  del_idx = begin.find('/');
  begin_d = static_cast<unsigned>(std::stoi(begin.substr(0, del_idx)));
  begin.erase(0, del_idx + 1);
  begin_y = std::stoi(begin);

  del_idx = end.find('/');
  end_m = static_cast<unsigned>(std::stoi(end.substr(0, del_idx)));
  end.erase(0, del_idx + 1);
  del_idx = end.find('/');
  end_d = static_cast<unsigned>(std::stoi(end.substr(0, del_idx)));
  end.erase(0, del_idx + 1);
  end_y = std::stoi(end);

  Date b_dt = Date(begin_y, begin_m, begin_d);
  Date e_dt = Date(end_y, end_m, end_d);
  events.push_back(Event(title, tag, b_dt, e_dt));
}

void Calendar::list_events() {
  for(size_t i = 0; i < events.size(); i++) {
    std::cout << std::setw(4) << events[i].get_tag()
              << ": " << events[i].get_begin()
              << " to " << std::setw(11) << events[i].get_end() 
              << "  " << events[i].get_title() << std::endl;
  }
}