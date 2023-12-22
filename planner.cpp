#include <iostream>
#include "cal.h"
#include "config.h"
#include <getopt.h>


struct option longOpts[] = {{"help", no_argument, nullptr, 'h'},
                              {"month", required_argument, nullptr, 'm'},
                              {"year", required_argument, nullptr, 'y'},
                              {"new", no_argument, nullptr, 'n'},
                              {"remove", optional_argument, nullptr, 'r'},
                              {"summary", no_argument, nullptr, 's'},
                              {"list", no_argument, nullptr, 'l'},
                              {nullptr, 0, nullptr, '\0'}};

int main(int argc, char** argv) {
  Calendar c = Calendar();
  std::chrono::sys_days today_serial;
  Date today;
  int option, param;

  {
    using namespace std::chrono;
    today_serial = sys_days{ floor<days>(system_clock::now()) };
    today = Date(today_serial);
  } 

  int begin_year = today.year();
  int end_year = today.year();
  unsigned begin_month = today.month();
  unsigned end_month = today.month();
  unsigned begin_day = 1;
  unsigned end_day = DAYS_IN_MONTH[end_month];
  if(end_month == 2 && !std::chrono::year{end_year}.is_leap()) --end_day;

  option = getopt_long(argc, argv, "hm:y:nr::sl", longOpts, 0);
  while(option) {
    if(option == -1) break;
    switch (option) {
    case 'h':
      //print help message
      //exit
      break;

    case 'm':
      param = atoi(optarg);
      if(param < 1 || param > 12) break; //TODO handle
      begin_month = static_cast<unsigned>(param);
      begin_day = 1;
      end_month = begin_month;
      end_day = (param == 2 && std::chrono::year{end_year}.is_leap()) ?
        DAYS_IN_MONTH[param] : DAYS_IN_MONTH[param]-1;
      break;
 
    case 'y':
      param = atoi(optarg);
      begin_year = param;
      end_year = param;
      end_day = (end_month == 2 && std::chrono::year{end_year}.is_leap()) ?
        DAYS_IN_MONTH[end_month] : DAYS_IN_MONTH[end_month]-1;
      break;
  
    case 'n':
      c.load_events(DEFAULT_SAVE_PATH);
      c.new_event();
      c.save_events(DEFAULT_SAVE_PATH);
      exit(0);
 
    case 'r':
      c.load_events(DEFAULT_SAVE_PATH);
      if(optarg) {
        if(optarg[0] != '=') {
          //TODO handle
          exit(0);
        }
        c.remove_event(optarg+1);
      } else {
        c.remove_event();
      }
      c.save_events(DEFAULT_SAVE_PATH);
      exit(0);

    case 's':
      today.change_day(0 - static_cast<int>(today.weekday_index()));
      begin_year = today.year();
      begin_month = today.month();
      begin_day = today.day();
      today.change_day((DAYS_IN_WEEK*2) - 1);
      end_year = today.year();
      end_month = today.month();
      end_day = today.day();
      break;

    case 'l':
      c.load_events(DEFAULT_SAVE_PATH);
      c.list_events();
      c.save_events(DEFAULT_SAVE_PATH);
      exit(0);

    default:
      break;
    }
    option = getopt_long(argc, argv, "hm:y:nr::sl", longOpts, 0);
  }

  c.load_events(DEFAULT_SAVE_PATH);
  c.set_range(begin_year, begin_month, begin_day, end_year, end_month, end_day);
  c.print();
  c.save_events(DEFAULT_SAVE_PATH);
  /*
  unsigned last_day_of_range;
  std::chrono::sys_days today_serial;
  Date today;

  switch (option)
  {
  case 'h':

  
  case 'm':
    //load events
    c.load_events(DEFAULT_SAVE_PATH);
    //set current focus to month specified in argument
    //set current focus to current month if not month passed
    {
      using namespace std::chrono;
      today_serial = sys_days{ floor<days>(system_clock::now()) };
      today = Date(today_serial);
      if(optarg) {
        std::cout << atoi(++optarg) << std::endl;
        today.change_month(atoi(optarg) - static_cast<int>(today.month()));
      }
    }
    last_day_of_range = static_cast<unsigned>(DAYS_IN_MONTH[today.month()]);
    if(!std::chrono::year{today.year()}.is_leap() && today.month() == 2) {
      --last_day_of_range;
    }
    c.set_range(today.year(), today.month(), 1, today.year(), today.month(), last_day_of_range);
    //print out calendar for focus month
    c.print();
    //save events
    c.save_events(DEFAULT_SAVE_PATH);
    break;

  case 'n':
    //load events
    c.load_events(DEFAULT_SAVE_PATH);
    //prompt user for new event info, create and add to cal
    c.new_event();
    //save events
    c.save_events(DEFAULT_SAVE_PATH);
    break;

  case 'r':
    //load events
    c.load_events(DEFAULT_SAVE_PATH);
    //determine event to remove from arg or prompt if not arg
    //remove event from calendar
    //save events
    c.save_events(DEFAULT_SAVE_PATH);
    break;

  case 'p':
    //load events
    c.load_events(DEFAULT_SAVE_PATH);
    //set current focus to current week and next week
    //print calendar for focus
    //save events
    c.save_events(DEFAULT_SAVE_PATH);
    break;

  case 'l':
    //load events
    c.load_events(DEFAULT_SAVE_PATH);
    //print list of events
    //save events
    c.save_events(DEFAULT_SAVE_PATH);
    break;

  default:
    break;
  }
  */
}
