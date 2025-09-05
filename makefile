
CXX = gcc
#gcc -Wall -Werror -o mscopier mscopier.cpp -lpthread -lstdc++
CXXFLAGS = -Wall -Werror
TARGETS = mmcopier mscopier
SRC_MM = mmcopier.cpp
SRC_MS = mscopier.cpp

# object file
OBJ_MM = $(SRC_MM:.cpp=.o)
OBJ_MS = $(SRC_MS:.cpp=.o)

all: $(TARGETS)

mmcopier: $(OBJ_MM)
	$(CXX) $(CXXFLAGS) -o $@ $^ -lpthread -lstdc++

mscopier: $(OBJ_MS)
	$(CXX) $(CXXFLAGS) -o $@ $^ -lpthread -lstdc++

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ_MM) $(OBJ_MS) $(TARGETS)