CXX = g++

CXXFLAGS = -Wall -Wextra -std=c++17 -Werror

LDLIBS = -lsfml-graphics -lsfml-window -lsfml-system  -lsfml-network

TARGET = game

SRCS = pingpong.cpp mainmenu.cpp networkmanager.cpp

OBJS = $(SRCS:.cpp=.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDLIBS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)

.PHONY: clean all
