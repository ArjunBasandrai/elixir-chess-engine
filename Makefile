SRC = src/utils/bits.cpp src/board/board.cpp src/hashing/hash.cpp src/move.cpp src/attacks/lookup.cpp src/movegen.cpp src/utils/perft.cpp

all: __compile __run

debug: __debug_compile __run

__compile:
	clang++ -Ofast -march=native -DNDEBUG -std=c++20 -o elixir.exe elixir.cpp $(SRC)

__debug_compile:
	clang++ -Og -g -std=c++20 -o elixir.exe elixir.cpp $(SRC)

__run:
	./elixir.exe