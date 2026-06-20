CXX = g++
CXXFLAGS = -std=c++11 -Wall -Wextra
TARGET = campus_analytics
SOURCES = main.cpp filehandler.cpp student_ops.cpp course_ops.cpp \
          attendance.cpp grades.cpp fee_tracker.cpp reports.cpp
OBJECTS = $(SOURCES:.cpp=.o)

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJECTS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJECTS) $(TARGET)

.PHONY: all clean
