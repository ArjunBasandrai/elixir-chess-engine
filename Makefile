SRC = src/utils/bits.cpp \
	  src/board/board.cpp \
	  src/hashing/hash.cpp \
	  src/move.cpp \
	  src/history.cpp \
	  src/attacks/lookup.cpp \
	  src/movegen.cpp \
	  src/utils/perft.cpp \
	  src/uci.cpp \
	  src/evaluate.cpp \
	  src/search.cpp \
	  src/utils/timer.cpp \
	  src/utils/masks.cpp \
	  src/bench/bench.cpp \
	  src/tt.cpp \
	  src/movepicker.cpp


CXX = clang++
EXE = elixir

SUFFIX = 
ifeq ($(OS),Windows_NT)
	SUFFIX = .exe
endif

EXE_NAME = $(EXE)$(SUFFIX)

all: __compile

debug: __debug_compile __run

test: __test_compile

__compile:
	$(CXX) -Ofast -march=native -DNDEBUG -std=c++20 -o $(EXE_NAME) elixir.cpp $(SRC)

__test_compile:
	$(CXX) -Ofast -march=native -DNDEBUG -std=c++20 -o elixir_test.exe elixir.cpp $(SRC)

__debug_compile:
	$(CXX) -Og -g -std=c++20 -o $(EXE_NAME) elixir.cpp $(SRC)

__run:
	./$(EXE_NAME)