
CXX = g++
#gcc -Wall -Werror -o mscopier mscopier.c -lpthread
CXXFLAGS = -Wall -Werror
TARGET = mscopier
SRC = mscopier.cpp

# object file
OBJ = $(SRC:.cpp=.o)

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJ) -lpthread

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(TARGET)