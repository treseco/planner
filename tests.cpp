#include <cassert>
#include <iostream>
#include <string>
#include <vector>

#include "cal.h"  
#include "datetime.h"


void date_tests() {
  std::cout << "Testing Date" << std::endl;
  Date default_ctor = Date();
  std::cout << "Expecting: 1970-1-1" << std::endl
            << "Result:    " << default_ctor << std::endl;
  Date copy_ctor = Date(default_ctor);
  copy_ctor.change_day(3);
  std::cout << "Expecting: 1970-1-4" << std::endl
            << "Result:    " << copy_ctor << std::endl;
  std::chrono::year_month_day copy_ymd = copy_ctor.ymd_obj();
  std::chrono::sys_days copy_sys_days{copy_ymd};
  Date sys_days_ctor = Date(copy_sys_days);
  sys_days_ctor.change_year(-3);
  std::cout << "Expecting: 1967-1-4" << std::endl
            << "Result:    " << sys_days_ctor << std::endl;
  Date manual_ctor = Date(2001, 12, 1);
  std::cout << "Expecting: 2001-12-1" << std::endl
            << "Result:    " << manual_ctor << std::endl;
  manual_ctor.change_month(15);
  std::cout << "Expecting: 2003-3-1" << std::endl
            << "Result:    " << manual_ctor << std::endl;
  try {
    Date invalid = Date(1999, 2, 30);
    assert(false);
  } catch (std::exception &ex) {
    std::cout << "Caught expected exception for invalid date passed to Date ctor:"  
              << std::endl << ex.what() << std::endl;
  }
  Date idx = Date(1999, 1, 1);
  std::chrono::weekday idx_wd{idx.weekday_index()};
  std::cout << idx << " was day " << idx.weekday_index() << " of the week" << std::endl;

  while(idx.year() < 2000) {
    std::cout << std::endl
              << idx.year() << "-" << idx.month() << std::endl
              << std::string(idx.weekday_index(), ' ');
    unsigned current_month = idx.month();
    while(idx.month() == current_month) {
        std::cout << '.';
        idx.change_day(1);
        if(idx.weekday_index() == 0 || idx.month() != current_month) std::cout << std::endl;
    }

  }
}



void timerange_tests() {
  Date b1 = Date(2001, 12, 25);
  Date e1 = Date(2002, 1, 1);
  Date in = Date(2001, 12, 30);
  Date out = Date(2001, 8, 8);
  TimeRange tr1 = TimeRange(b1, e1);
  Date b2 = Date(2020, 4, 29);
  Date e2 = Date(2020, 5, 5);
  std::chrono::sys_days b2_days{b2.ymd_obj()};
  std::chrono::sys_days e2_days{e2.ymd_obj()};
  TimeRange tr2 = TimeRange(b2_days, e2_days);
  std::cout << b1 << "  -  " << e1 << std::endl;
  std::cout << tr1.get_begin() << "  -  " << tr1.get_end() << std::endl;
  assert(b1 == tr1.get_begin());
  assert(e1 == tr1.get_end());
  assert(tr1.contains(in));
  assert(!tr1.contains(out));
  std::cout << b2 << "  -  " << e2 << std::endl;
  std::cout << tr2.get_begin() << "  -  " << tr2.get_end() << std::endl;
  assert(b2 == tr2.get_begin());
  assert(e2 == tr2.get_end());
  try {
    TimeRange tr3 = TimeRange(e1, b1);
    assert(false);
  } catch (std::exception &ex) {
    std::cout << "Caught expected exception for invalid dates passed to TimeRange ctor:"  
              << std::endl << ex.what() << std::endl; 
  }
  try {
    TimeRange tr4 = TimeRange(e2_days, b2_days);
    assert(false);
  } catch (std::exception &ex) {
    std::cout << "Caught expected exception for invalid dates passed to TimeRange ctor:"  
              << std::endl << ex.what() << std::endl; 
  }
  TimeRange tr5 = TimeRange(Date(2222, 12, 12), Date(2222, 12, 12));
}

void event_tests() {
  Date b1 = Date(2023, 1, 1);
  Date e1 = Date(2023, 2, 1);
  Date b2 = Date(2023, 1, 1);
  Date e2 = Date(2023, 9, 30);
  std::chrono::sys_days b2_days{b2.ymd_obj()};
  std::chrono::sys_days e2_days{e2.ymd_obj()};
  Event ev1 = Event("Event 1 Title", "TAG1", b1, e1);
  Event ev2 = Event("Event 2 Title", "LONGTAG", b2_days, e2_days);
  Event ev3 = Event();
  assert(ev1.get_tag() == "TAG1");
  assert(ev1.get_title() == "Event 1 Title");
  assert(ev2.get_tag() == "LONG");
  assert(ev2.get_title() == "Event 2 Title");
  assert(ev3.get_tag() == "TAG");
  assert(ev3.get_title() == "TITLE");
}

void calendarrange_tests() {
  Date b2 = Date(2023, 1, 1);
  Date e2 = Date(2023, 1, 31); 
  Date b3 = Date(2023, 2, 1);
  Date e3 = Date(2023, 2, 28);
  CalendarRange cr1 = CalendarRange();
  CalendarRange cr2 = CalendarRange(b2, e2);
  CalendarRange cr3 = CalendarRange(b3, e3);
  assert(cr1.get_begin() == Date(1970, 1, 1));
  assert(cr1.get_end() == Date(1970, 1, 1));
  std::vector<Event> events;
  Date b4b = Date(2022, 12, 1);
  Date b4e = Date(2022, 12, 25);
  Date xbb = Date(2022, 12, 29);
  Date xbe = Date(2023, 1, 5);
  Date wrb = Date(2023, 1, 2);
  Date wre = Date(2023, 1, 17);
  Date xeb = Date(2023, 1, 27);
  Date xee = Date(2023, 2, 3);
  Date atb = Date(2023, 2, 7);
  Date ate = Date(2023, 2, 15);
  events.emplace_back(Event("Before", "b4", b4b, b4e));
  events.emplace_back(Event("Across begin", "xb", xbb, xbe));
  events.emplace_back(Event("Within range", "wr", wrb, wre));
  events.emplace_back(Event("Across end", "xe", xeb, xee));
  events.emplace_back(Event("After", "aftr", atb, ate));
    
  cr2.set_events(&events);
  cr3.set_events(&events);
  std::cout << cr2.print_cal();
  std::cout << cr3.print_cal();
}

int main() {
  date_tests();
  timerange_tests();
  event_tests();
  calendarrange_tests();
  return 0;
}