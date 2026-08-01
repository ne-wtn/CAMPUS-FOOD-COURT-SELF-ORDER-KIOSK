# =============================================================================
#  Makefile - for team members not on Windows (macOS, Linux, WSL)
#  Windows users can just run build.bat instead.
#
#    make          build kiosk
#    make run      build and start the system
#    make clean    delete the built files
# =============================================================================

CXX      := g++
CXXFLAGS := -std=c++11 -Wall -Wextra
TARGET   := kiosk
SOURCES  := $(wildcard *.cpp)
OBJECTS  := $(SOURCES:.cpp=.o)

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CXX) $(CXXFLAGS) -o $@ $^
	@echo "BUILD OK -> ./$(TARGET)"

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(OBJECTS) $(TARGET) $(TARGET).exe

.PHONY: all run clean
