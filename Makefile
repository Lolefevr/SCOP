# Variables
CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra
LIBS = -lGL -lGLEW -lglfw
INCLUDES = -I./include
SRC_DIR = .
OBJ_DIR = obj
TARGET = OpenGL_Project

# Sources and objects
SRCS = $(wildcard $(SRC_DIR)/*.cpp)
OBJS = $(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DIR)/%.o, $(SRCS))

# Rules
all: $(TARGET)

$(TARGET): $(OBJS) | $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LIBS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp | $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

clean:
	rm -rf $(OBJ_DIR)

fclean: clean
	rm -rf $(TARGET)

re: fclean all

.PHONY: all clean fclean re
