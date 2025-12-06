CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra

TARGET = syslog-analyzer
SOURCES = src/main.cpp
OBJECTS = $(SOURCES:.cpp=.o)

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJECTS)

clean:
	del /Q $(OBJECTS) $(TARGET).exe 2>nul || true
