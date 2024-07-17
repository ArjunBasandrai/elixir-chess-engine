SRC := $(wildcard src/*.cpp) $(wildcard src/**/*.cpp)

CXX = g++
EXE = elixir

SUFFIX = 
ifeq ($(OS),Windows_NT)
	SUFFIX = .exe
endif

ifeq ($(UNAME_S), Darwin)
	STACK = -Wl,-stack_size,0x1000000
else ifeq ($(UNAME_S), Linux)
	STACK = -Wl,-zstack_size=0x1000000
else ifeq ($(OS), Windows_NT)
	ifeq ($(CXX), clang++)
		STACK = -Wl,/STACK:16777216
	else ifeq ($(CXX), g++)
		STACK = -Wl,--stack,16777216
	endif
endif


EXE_NAME = $(EXE)$(SUFFIX)

all: __compile

debug: __debug_compile __run

tune: __tune_compile

texel: __texel_compile __texel_run

__compile:
	g++ -Ofast -march=native -DNDEBUG -DEVALFILE=\"nnue.bin\" -std=c++20 $(STACK) -o $(EXE_NAME) elixir.cpp $(SRC)

__tune_compile:
	g++ -Ofast -march=native -DNDEBUG -DUSE_TUNE -DEVALFILE=\"nnue.bin\" -std=c++20 $(STACK) -o $(EXE_NAME) elixir.cpp $(SRC)

__texel_compile:
	g++ -Ofast -march=native -DNDEBUG -DTEXEL -DEVALFILE=\"nnue.bin\" -std=c++20 $(STACK) -o $(EXE_NAME) elixir.cpp $(SRC)

__debug_compile:
	g++ -Og -g -DEVALFILE=\"nnue.bin\" -std=c++20 $(STACK) -o $(EXE_NAME) elixir.cpp $(SRC)

__run:
	./$(EXE_NAME)

__texel_run:
	./$(EXE_NAME) texel