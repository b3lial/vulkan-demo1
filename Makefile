CXX = g++
CXXFLAGS = -std=c++17 -Wall -O2
LDFLAGS = -lvulkan -lglfw

SRC_DIR = src
SRCS = $(wildcard $(SRC_DIR)/*.cpp)
TARGET = triangle

all: $(TARGET)

$(TARGET): $(SRCS)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

clean:
	rm -f $(TARGET)
