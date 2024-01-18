#ifndef DATE_H
#define DATE_H

#include <chrono>
#include <string>
#include <vector>
  
#define DAYS_IN_WEEK 7
static const unsigned DAYS_IN_MONTH[13] = {0, 31, 29, 31, 30, 31, 30,
                                              31, 31, 30, 31, 30, 31};

static const std::string MONTH_ABREV[13] = {"", "JAN", "FEB", "MAR", "APR", "MAY", "JUN",
                                                "JUL", "AUG", "SEP", "OCT", "NOV", "DEC"};

class Date {
private:
  std::chrono::year_month_day ymd;
  long int serial;

  //updates serial value for ymd
  void update_serial();
  //corrects invalid dates in ymd
  void validate_ymd();

public:
  //=== Constructors ===

  //default initialize to 1970-1-1
  Date();
  //copy constructor
  //Date(const Date& dt);
  //initialize from std::chrono::time_point object
  Date(std::chrono::sys_days &d);
  //initialize from year, month, day objects
  Date(int y, unsigned m, unsigned d);
  //move constructor
  //Date& operator=(Date&&);

  //=== Accessors ===

  //return date as days since epoch (1970-1-1)
  long int serial_time() const; //return date as days since epoch 
  //return day
  unsigned day() const;
  //return month
  unsigned month() const;
  //return year
  int year() const;
  //retunr std::chrono::year_month_day object
  std::chrono::year_month_day ymd_obj() const;
  //return day of week S=0, M=1, T=2, W=3, T=4, F=5, S=6
  unsigned weekday_index() const;
  //return date as string representing tz timestamp
  std::string to_tz_tstamp() const;

  //=== Modifiers ===

  //add signed value deleta to days and validate date
  void change_day(int delta);
  //add signed value delta to month and validate date
  void change_month(int delta);
  //add signed value delta to year and validate date
  void change_year(int delta);
  //set date to the beginning of the current week (prev sunday).
  void snap_to_wk_begin();
  //set date to the end of the current week (next sunday).
  void snap_to_wk_end();

  //=== Operators ===

  //equals operator
  bool operator==(const Date &rhs) const;
  //increment operators
  Date& operator++ ();
  Date operator++ (int);
  //spaceship operator
  std::strong_ordering operator<=>(const Date &rhs) const;
  //stream operator, outputs date as YYYY-MM-DD
  friend std::ostream & operator<< (std::ostream &out, const Date &d);
};


class TimeRange {
private:
  Date begin;
  Date end;
public:

  // === Constructors ===

  //Default initialize to 1970-1-1 - 1970-1-1
  TimeRange();
  //initialize from two std::chrono::time_point objects
  TimeRange(std::chrono::sys_days &begin, std::chrono::sys_days &end);
  //initialize from Date objects
  TimeRange(const Date &begin, const Date &end);

  // === Accessors ===

  //return begin Date object
  const Date &get_begin() const;
  //retrun end Date object
  const Date &get_end() const;
  //return true if TimeRange contains paramteter date
  bool contains(const Date &d) const;
};

class Event : public TimeRange {
private:
  std::string title;
  std::string tag;  
public:

  // === Constructors ===

  //Default initialize with title = "TITLE" & tag = "TAG"
  Event();
  //initialize from two std::chrono::sys_days objects
  Event(std::string title, std::string tag, std::chrono::sys_days &begin, std::chrono::sys_days &end);
  //initialize from two Date objects
  Event(std::string title, std::string tag, Date &begin, Date &end);

  // === Accessors ===

  //return event title
  std::string get_title();
  //return event tag
  std::string get_tag();

  struct {
    bool operator()(Event x, Event y) const {
      return x.tag.compare(y.tag) < 0;
    }
  }static tag_alpha;
};


class CalendarRange : public TimeRange {
private:
  std::vector<Event> events_in_range;
  size_t max_concurrent_events;

  struct {
    bool operator()(Event x, Event y) {
      if (x.get_begin() == y.get_begin())
        return x.get_end() < y.get_end();
      else
        return x.get_begin() < y.get_begin();
    }
  } static starts_before;

  std::string gen_key();

public:

  // === Constructors ===

  //default
  CalendarRange();
  //initialize with Dates begin & end
  CalendarRange(Date &begin, Date &end);

  // === Modifiers ===

  //poulate events_in_rannge with events 
  void set_events(std::vector<Event> * events);


  std::string print_cal(); //print out calendar events over calendar range
};

Date get_todays_date();

#endif

