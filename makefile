
CXX = g++
#gcc -Wall -Werror -o mscopier mscopier.c -lpthread
CXXFLAGS = -Wall -Werror
TARGETS = mmcopier mscopier
SRC_MM = mmcopier.cpp
SRC_MS = mscopier.cpp

# object file
OBJ_MM = $(SRC_MM:.cpp=.o)
OBJ_MS = $(SRC_MS:.cpp=.o)

all: $(TARGETS)

mmcopier: $(OBJ_MM)
	$(CXX) $(CXXFLAGS) -o $@ $^ -lpthread

mscopier: $(OBJ_MS)
	$(CXX) $(CXXFLAGS) -o $@ $^ -lpthread

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ_MM) $(OBJ_MS) $(TARGETS)