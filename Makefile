CXX = clang++
MODE = -O3
_OS = $(shell uname)
_OBJ = libWFC
OBJ = 

SRC = $(wildcard src/**/*.cpp) $(wildcard src/*.cpp)

INCLUDE = -Iinclude -Isrc
CXXFLAGS = -std=c++20 $(MODE) 

LDFLAGS = 

ifeq ($(_OS), Darwin)
	CXX = clang++
	OBJ = $(_OBJ).dylib
	LDFLAGS += -dynamiclib -fPIC
else ifeq ($(_OS), Linux) 
	CXX = g++
	OBJ = $(_OBJ).so
	LDFLAGS += -shared -fPIC
else ifeq ($(OS), Windows_NT)
	CXX = g++
	OBJ = $(_OBJ).dll
	LDFLAGS += -shared -fPIC
endif

OBJS = $(addsuffix .o, $(basename $(notdir $(SRC))))

%.o: src/%.cpp
	@$(CXX) -c -o $@ $< $(INCLUDE) $(CXXFLAGS)
	@echo "Built $@"
	
%.o: src/**/%.cpp
	@$(CXX) -c -o $@ $< $(INCLUDE) $(CXXFLAGS)
	@echo "Built $@"

all: $(OBJ)
	@rm $(OBJS)

$(OBJ):	$(OBJS)
	@$(CXX) -o $(OBJ) $^ $(LDFLAGS)
	@cp $(OBJ) bin/$(OBJ)

clean:
	@rm -f $(OBJS) $(OBJ)

add:
	@cpp-pkgm -p $(PWD) WFC 0 4
	
update:
	@cpp-pkgm -r WFC
	@cpp-pkgm -p $(PWD) WFC 0 4

