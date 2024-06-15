SRC := $(wildcard src/*.cpp) $(wildcard src/**/*.cpp)

CXX = clang++
EXE = elixir

SUFFIX = 
ifeq ($(OS),Windows_NT)
	SUFFIX = .exe
endif

EXE_NAME = $(EXE)$(SUFFIX)

all: __compile

debug: __debug_compile __run

tune: __tune_compile

__compile:
	$(CXX) -Ofast -march=native -DNDEBUG -std=c++20 -o $(EXE_NAME) elixir.cpp $(SRC)

__tune_compile:
	$(CXX) -Ofast -march=native -DNDEBUG -DUSE_TUNE -std=c++20 -o $(EXE_NAME) elixir.cpp $(SRC)

__debug_compile:
	$(CXX) -Og -g -std=c++20 -o $(EXE_NAME) elixir.cpp $(SRC)

__run:
	./$(EXE_NAME)