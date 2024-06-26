#include <algorithm>
#include <exception>
#include <iostream>
#include <iomanip>
#include "datetime.h"
#include "config.h"
#include "color.h"

// === Date ===
Date::Date() : ymd{std::chrono::year(1970), std::chrono::month(1), std::chrono::day(1)}, serial(0) {}

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

long int Date::serial_time() const { 
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

std::string Date::to_tz_tstamp() const {
  std::string year = std::to_string(this->year());
  year = std::string(4-year.length(),'0').append(year);
  std::string month = std::to_string(this->month());
  month = std::string(2-month.length(),'0').append(month);
  std::string day = std::to_string(this->day());
  day = std::string(2-day.length(),'0').append(day);
  return year + month + day + "T000000Z";
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

void Date::snap_to_wk_begin() {
  change_day(-static_cast<int>(weekday_index()));
}

void Date::snap_to_wk_end() {
  snap_to_wk_begin();
  change_day(DAYS_IN_WEEK);
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

Date& Date::operator++ () {
  this->change_day(1);
  return *this;
}

Date Date::operator++ (int) {
  Date result(*this);
  ++(*this);
  return result;
}

std::strong_ordering Date::operator<=>(const Date &rhs) const {
  return serial_time() <=> rhs.serial_time();
}

std::ostream & operator<< (std::ostream &out, const Date &d) {
  out << std::setfill('0') 
      << std::setw(4) << d.year() << "-" 
      << std::setw(2) << d.month() << "-" 
      << std::setw(2) << d.day()
      << std::setfill(' ');
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
CalendarRange::CalendarRange() : TimeRange(), max_concurrent_events(0) {}

CalendarRange::CalendarRange(Date &begin, Date &end) 
  : TimeRange(begin, end), max_concurrent_events(0) {}

std::string CalendarRange::gen_key() {
  std::string key = "";
  for (size_t i = 0; i < events_in_range.size(); i++) {
    long int color_idx = i % NUM_COLORS;
    key += color(events_in_range[i].get_tag(), bg_colors[color_idx]+BLACK);
    key += ": ";
    key += events_in_range[i].get_title() + '\n';
  }
  return key;
}

std::string CalendarRange::print_cal() {

  std::string empty_day(DEFAULT_DAY_WIDTH + 1, ' ');
  Date today = get_todays_date();

  std::string cal = "";
  std::string key = gen_key();

  //get full week containing begin
  Date wk_begin = get_begin();
  Date wk_end   = get_begin();
  wk_begin.snap_to_wk_begin();
  wk_end.snap_to_wk_end();

  std::string separator = "";
  std::string date_row = "";
  std::vector<std::pair<bool, size_t> > event_slots;
  std::vector<std::string> event_strings;

  //setup event slots
  size_t next_to_start = 0;
  for(size_t i = 0; i < max_concurrent_events; ++i) {
    event_slots.emplace_back(std::make_pair(false, 0));
    event_strings.emplace_back("");
  }
  
  //setup initial separator
  for(unsigned i = 0; i < DAYS_IN_WEEK; ++i) {
    if(i < get_begin().weekday_index()) {
      separator += empty_day;
    } else {
      separator += "+" + std::string(DEFAULT_DAY_WIDTH, '-');
    }
  }
  
  do {
    // add a separator prior to each week in the range.
    cal += separator + "+\n";
    separator = "";

    for(Date d = wk_begin; d < wk_end && d <= get_end(); ++d) {

      // extend the separator for each day in the week
      // this separator goes below the current week
      separator += "+";
      separator += std::string(DEFAULT_DAY_WIDTH, '-');

      if(d < get_begin()) {
        // handle range starting in the middle of a week
        date_row += empty_day;
        for(size_t i = 0; i < event_strings.size(); ++i) {
          event_strings[i] += empty_day;
        }
      } else {
        //set date_row for day d
        std::string date_str = std::to_string(d.day());
        if(d.day() == 1) {
          date_str += " " + MONTH_ABREV[d.month()];
        }
        std::string spaces = std::string(DEFAULT_DAY_WIDTH-1-(date_str.length()), ' ');
        if(d == today) date_str = color(date_str, RED);

        date_row += "| " + date_str + spaces;

        //assign starting events an event slot
        while(!events_in_range.empty() && events_in_range[next_to_start].contains(d)) {
          for(size_t i = 0; i < event_slots.size(); ++i) {
            auto &[used, event_idx] = event_slots[i];
            if(!used) {
              used = true;
              event_idx = next_to_start;
              break;
            }
          }
          ++next_to_start;
        }
        
        //update event strings
        for(size_t i = 0; i < event_slots.size(); ++i) {
          auto &[used, event_idx] = event_slots[i];
          std::string tag;
          std::string fill;
          long int color_idx = event_idx % NUM_COLORS;
          event_strings[i] += "|";
          if(used) {
            Event current_event = events_in_range[event_idx];
             
            if(current_event.get_begin() == d) {
              tag  = color(current_event.get_tag(), bg_colors[color_idx]+BLACK);
              fill = color(std::string(DEFAULT_DAY_WIDTH-current_event.get_tag().length()-2, '='), fg_colors[color_idx]);
              if(current_event.get_end() == d) {
                fill += color("*", fg_colors[color_idx]);
                used = false;
              } else {
                fill += color("=", fg_colors[color_idx]);
              }
              event_strings[i] += color("*", fg_colors[color_idx]) + tag + fill;
            } else if (current_event.get_end() == d) {
              fill = color(std::string(DEFAULT_DAY_WIDTH-1, '=')+"*", fg_colors[color_idx]);
              event_strings[i] += fill;
              used = false;
            } else {
              fill = color(std::string(DEFAULT_DAY_WIDTH, '='), fg_colors[color_idx]);
              event_strings[i] += fill;
            }
          } else {
            event_strings[i] += std::string(DEFAULT_DAY_WIDTH, ' ');
          }
        }
      }
    }
    cal += date_row + "|\n";
    date_row = "";
    for(size_t i = 0; i < event_strings.size(); ++i) {
      cal += event_strings[i] + "|\n";
      event_strings[i] = "";
    }
    wk_begin.change_day(DAYS_IN_WEEK);
    wk_end.change_day(DAYS_IN_WEEK);
  } while(wk_begin <= get_end());
  cal += separator + "+\n" + key;
  return cal;
}

void CalendarRange::set_events(std::vector<Event> *events) {
  //populate events_in range with valid events from 'events'
  for(size_t i = 0; i < events->size(); ++i) {
    Event e = (*events)[i];
    if (e.get_begin() <= this->get_end() && e.get_end() >= this->get_begin()) {
      events_in_range.push_back((*events)[i]);
    }
  }

  //sort valid events by start time
  std::sort(events_in_range.begin(), events_in_range.end(), starts_before);

  //calculate max_concurrent_events in events_in_range
  for(Date d = get_begin(); d <= get_end(); ++d) {
    size_t events_on_day = 0;
    for(size_t i = 0; i < events_in_range.size(); ++i) {
      if(events_in_range[i].contains(d)) {
        ++events_on_day;
      }
    }
    if(events_on_day > max_concurrent_events) {
      max_concurrent_events = events_on_day;
    }
  }
}

Date get_todays_date() {
  using namespace std::chrono;
  sys_days today_serial = sys_days{floor<days>(system_clock::now())};
  return Date(today_serial);
}
