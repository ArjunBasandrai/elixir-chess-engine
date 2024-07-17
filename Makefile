SRC := $(wildcard src/*.cpp) $(wildcard src/**/*.cpp)

CXX = clang++
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
	$(CXX) -Ofast -march=native -DNDEBUG -std=c++20 $(STACK) -o $(EXE_NAME) elixir.cpp $(SRC)

__tune_compile:
	$(CXX) -Ofast -march=native -DNDEBUG -DUSE_TUNE -std=c++20 $(STACK) -o $(EXE_NAME) elixir.cpp $(SRC)

__texel_compile:
	$(CXX) -Ofast -march=native -DNDEBUG -DTEXEL -std=c++20 $(STACK) -o $(EXE_NAME) elixir.cpp $(SRC)

__debug_compile:
	$(CXX) -Og -g -std=c++20 $(STACK) -o $(EXE_NAME) elixir.cpp $(SRC)

__run:
	./$(EXE_NAME)

__texel_run:
	./$(EXE_NAME) texel