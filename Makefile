include ext/ext.mk

CXX = clang++
MODE = -g3
_OS = $(shell uname)
_OBJ = WFC
OBJ = 

SRC = $(wildcard src/**/*.cpp) $(wildcard src/*.cpp)

INCLUDE = -Iinclude -Isrc
CXXFLAGS = -std=c++20 $(MODE) $(EXT_CFLAGS)

LDFLAGS = $(EXT_LDFLAGS) -lsdl2 -lsdl2_image -lyaml-cpp

ifeq ($(_OS), Darwin)
	CXX = clang++
	OBJ = $(_OBJ)
# LDFLAGS += -dynamiclib -fPIC
else ifeq ($(_OS), Linux) 
	CXX = g++
	OBJ = $(_OBJ)
# LDFLAGS += -shared -fPIC
else ifeq ($(OS), Windows_NT)
	CXX = g++
	OBJ = $(_OBJ).exe
# LDFLAGS += -shared -fPIC
endif

OBJS = $(addsuffix .o, $(basename $(notdir $(SRC))))

%.o: src/%.cpp
	@$(CXX) -c -o $@ $< $(INCLUDE) $(CXXFLAGS)
	@echo "Built $@"
	
%.o: src/**/%.cpp
	@$(CXX) -c -o $@ $< $(INCLUDE) $(CXXFLAGS)
	@echo "Built $@"

$(OBJ):	$(OBJS)
	@$(CXX) -o $(OBJ) $^ $(LDFLAGS) $(MODE)
	@cp $(OBJ) bin/$(OBJ)

clean:
	@rm -f $(OBJS) $(OBJ)
