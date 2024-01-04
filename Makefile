# Makefile

# Compiler
CXX = g++

# Compiler flags (including debug info)
CXXFLAGS   = -std=c++20 -Wall -Werror -Wconversion -Wextra
DBGFLAGS   = -g3 -DDEBUG # defines DEBUG for #ifdef DEBUG ... #endif
EXECUTABLE = planner
SOURCES    = cal.cpp datetime.cpp planner.cpp color.cpp
TESTSORCES = tests.cpp


# Compile planner
planner: $(SOURCES)
	$(CXX) $(CXXFLAGS) -O3 $(SOURCES) -o $(EXECUTABLE)

# Compile planner with debug symbols
debug: $(SOURCES)
	$(CXX) $(CXXFLAGS) $(DBGFLAGS) $(SOURCES) -o $(EXECUTABLE)_debug

# Compiler planner tests
test: $(TESTSORCES) datetime.cpp
	$(CXX) $(CXXFLAGS) $(DBGFLAGS) datetime.cpp cal.cpp $(TESTSORCES) -o $(EXECUTABLE)_tests

# Remove anything created by a makefile
clean:
	rm -f *.o *.out planner planner_debug planner_tests
	rm -rf *.dSYM
