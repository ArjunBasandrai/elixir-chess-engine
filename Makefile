SRC = src/utils/bits.cpp src/board/board.cpp src/hashing/hash.cpp src/move.cpp src/attacks/lookup.cpp src/movegen.cpp src/utils/perft.cpp

all: __compile __run

debug: __debug_compile __run

__compile:
	clang++ -Ofast -DNDEBUG -std=c++20 -o elixir.exe elixir.cpp $(SRC)

__debug_compile:
	clang++ -Ofast -std=c++20 -o elixir.exe elixir.cpp $(SRC)

__run:
	./elixir.exe