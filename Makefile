SRC = src/utils/bits.cpp \
	  src/board/board.cpp \
	  src/hashing/hash.cpp \
	  src/move.cpp \
	  src/attacks/lookup.cpp \
	  src/movegen.cpp \
	  src/utils/perft.cpp \
	  src/uci.cpp \
	  src/evaluate.cpp \
	  src/search.cpp \
	  src/utils/timer.cpp

all: __compile __run

debug: __debug_compile __run

test: __test_compile

__compile:
	clang++ -Ofast -march=native -DNDEBUG -std=c++20 -o elixir.exe elixir.cpp $(SRC)

__test_compile:
	clang++ -Ofast -march=native -DNDEBUG -std=c++20 -o elixir_test.exe elixir.cpp $(SRC)

__debug_compile:
	clang++ -Og -g -std=c++20 -o elixir.exe elixir.cpp $(SRC)

__run:
	./elixir.exe