#include <algorithm>
#include <exception>
#include <iostream>
#include "datetime.h"
#include "config.h"

// === Date ===
Date::Date() : ymd{std::chrono::year(1970), std::chrono::month(1), std::chrono::day(1)}, serial(0) {}

/*
Date::Date(const Date &dt) : ymd(dt.ymd) {
  if(!ymd.ok()) {
    throw std::invalid_argument("Invalid Date");
  }
  update_serial();
}
*/

Date::Date(std::chrono::sys_days &d) : ymd{d} {
  update_serial();
}

Date::Date(int y, unsigned m, unsigned d) 
    : ymd{std::chrono::year{y}, std::chrono::month{m}, std::chrono::day{d}} {
  if(!ymd.ok()) {
    throw std::invalid_argument("Invalid Date");
  }
  update_serial();
}

int Date::serial_time() const { 
  return serial; 
}

unsigned Date::day() const {
  return static_cast<unsigned>(ymd.day());
}

unsigned Date::month() const {
  return static_cast<unsigned>(ymd.month());
}

int Date::year() const {
  return static_cast<int>(ymd.year());
}

std::chrono::year_month_day Date::ymd_obj() const {
  return ymd;
}

unsigned Date::weekday_index() const {
  std::chrono::weekday wd{ std::chrono::sys_days{ymd} };
  return wd.c_encoding();
}

void Date::change_day(int delta) {
  auto result = std::chrono::sys_days{ymd};
  if(delta < 0) {
    result -= std::chrono::days(abs(delta));
  } else {
    result += std::chrono::days(delta);
  }
  ymd = result;
  validate_ymd();
  update_serial();
}

void Date::change_month(int delta) {
  if(delta < 0) {
    ymd -= std::chrono::months(abs(delta));
  } else {
    ymd += std::chrono::months(delta);
  }
  validate_ymd();
  update_serial();
}

void Date::change_year(int delta) {
  if(delta < 0) {
    ymd -= std::chrono::years(abs(delta));
  } else {
    ymd += std::chrono::years(delta);
  }
  validate_ymd();
  update_serial();
}

void Date::update_serial() {
  serial = std::chrono::sys_days(ymd).time_since_epoch().count();
}

void Date::validate_ymd() {
  if(!ymd.ok()) {
    ymd += std::chrono::months{0};
    ymd =  std::chrono::sys_days{ymd};
  }
}

bool Date::operator==(const Date &rhs) const {
  return this->serial_time() == rhs.serial_time();
}

std::strong_ordering Date::operator<=>(const Date &rhs) const {
  return serial_time() <=> rhs.serial_time();
}

std::ostream & operator<< (std::ostream &out, const Date &d) {
  out << d.year() << "-" << d.month() << "-" << d.day();
  return out;
}


// === TimeRange ===
TimeRange::TimeRange() : begin(Date()), end(Date()) {}

TimeRange::TimeRange(std::chrono::sys_days &begin, std::chrono::sys_days &end)
  : begin(begin), end(end) {
    if(begin > end) throw std::invalid_argument("Invalid TimeRange");
}

TimeRange::TimeRange(const Date &begin, const Date &end) : begin(begin), end(end) {
  if(begin > end) throw std::invalid_argument("Invalid TimeRange");
}

const Date &TimeRange::get_begin() const { return begin; }

const Date &TimeRange::get_end() const { return end; }

bool TimeRange::contains(const Date &d) const { return (!(d < begin || d > end)); }

// === Event ===
Event::Event() : TimeRange(), title("TITLE"), tag("TAG") {}

Event::Event(std::string title, std::string tag, std::chrono::sys_days &begin,
             std::chrono::sys_days &end)
    : TimeRange(begin, end), title(title), tag(tag.substr(0, 4)) {}

Event::Event(std::string title, std::string tag, Date &begin, Date &end)
    : TimeRange(begin, end), title(title), tag(tag.substr(0, 4)) {}

std::string Event::get_title() { return title; }

std::string Event::get_tag() { return tag; }


// === CalendarRange ===
CalendarRange::CalendarRange() : TimeRange() {}

CalendarRange::CalendarRange(Date &begin, Date &end) : TimeRange(begin, end) {}

std::string CalendarRange::print_cal() {

  std::string cal = "";

  //set key
  std::string key = "";
  for (size_t i = 0; i < events_in_range.size(); i++) {
    key += events_in_range[i].get_tag() + ": " +
           events_in_range[i].get_title() + '\n';
  }

  std::string empty_day(DEFAULT_DAY_WIDTH+1, ' ');
  Date wk_begin = get_begin();
  wk_begin.change_day(-static_cast<int>((get_begin().weekday_index())));
  Date wk_end = wk_begin;
  wk_end.change_day(DAYS_IN_WEEK);


  std::string separator = "";
  std::string date_row = "";
  std::vector<std::string> event_strings;
  for(size_t i = 0; i < events_in_range.size(); ++i) {
    event_strings.emplace_back("");
  }
  
  for(unsigned i = 0; i < DAYS_IN_WEEK; ++i) {
    if(i < get_begin().weekday_index()) {
      separator += empty_day;
    } else {
      separator += "+" + std::string(DEFAULT_DAY_WIDTH, '-');
    }
  }

  do {
    cal += separator + "+\n";
    separator = "";
    for(Date d = wk_begin; d < wk_end && d <= get_end(); d.change_day(1)) {
      
      separator += "+" + std::string(DEFAULT_DAY_WIDTH, '-');
      if(d < get_begin()) {

        date_row += empty_day;
        for(size_t i = 0; i < event_strings.size(); ++i) {
          event_strings[i] += empty_day;
        }

      } else {

        date_row += "| " + std::to_string(d.day()) 
                + std::string(DEFAULT_DAY_WIDTH-1 - (std::to_string(d.day())).length(), ' ');
        
        for(size_t i = 0; i < events_in_range.size(); ++i) {
          if(events_in_range[i].contains(d)) {
            if(d == events_in_range[i].get_begin()) {
              event_strings[i] += "|*" + events_in_range[i].get_tag()
              + std::string(DEFAULT_DAY_WIDTH-1 - (events_in_range[i].get_tag()).length(), '=');
            } else if (d == events_in_range[i].get_end()) {
              event_strings[i] += "|" + std::string(DEFAULT_DAY_WIDTH-1, '=') + "*";
            } else {
              event_strings[i] += "|" + std::string(DEFAULT_DAY_WIDTH, '=');
            }
          } else {
            event_strings[i] += "|" + std::string(DEFAULT_DAY_WIDTH, ' ');
          }
        }

      }
    }
    //cal += separator + "+\n";
    cal += date_row + "|\n";
    date_row = "";
    for(size_t i = 0; i < event_strings.size(); ++i) {
      cal += event_strings[i] + "|\n";
      event_strings[i] = "";
    }
    wk_begin.change_day(DAYS_IN_WEEK);
    wk_end.change_day(DAYS_IN_WEEK);
  } while(wk_begin <= get_end());
  cal += separator + "+\n";
  /*
  unsigned wk_start_offset = get_begin().weekday_index();
  Date d = get_begin();

  cal += std::string(wk_start_offset * (DEFAULT_DAY_WIDTH+1), ' ');
  do {
    cal += "+" + std::string(DEFAULT_DAY_WIDTH, '-');
    d.change_day(1);
  } while (d.weekday_index() != 0);
  cal += "+\n";
  */


  cal += key;
  return cal;
}

void CalendarRange::set_events(std::vector<Event> *events) {
  for (size_t i = 0; i < events->size(); i++) {
    Event e = (*events)[i];
    if (e.get_begin() < this->get_end() && e.get_end() >= this->get_begin()) {
      events_in_range.push_back((*events)[i]);
    }
  }

  std::sort(events_in_range.begin(), events_in_range.end(), starts_before);
}