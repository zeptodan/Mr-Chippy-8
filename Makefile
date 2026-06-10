CXX = g++

CXXFLAGS = -Wall -Wextra -Werror

LDLIBS = -lSDL2

SRCS = main.cpp graphics.cpp chip8.cpp

BUILD_DIR = build



OBJS = $(SRCS:%.cpp=$(BUILD_DIR)/%.o)


all: Mr-Chippy-8 

Mr-Chippy-8: $(OBJS)

	$(CXX) $(OBJS) -o $@ $(LDLIBS)

$(BUILD_DIR)/%.o: %.cpp
	mkdir -p $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@


clean:
	rm -rf $(BUILD_DIR)
	rm -f Mr-Chippy-8