CXX = g++

CXXFLAGS = -Wall -Wextra -std=c++17

LDLIBS = -lsfml-graphics -lsfml-window -lsfml-system -lsfml-audio

TARGET = game

SRCS = pingpong.cpp Menu.cpp

OBJS = $(SRCS:.cpp=.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDLIBS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)

.PHONY: clean all