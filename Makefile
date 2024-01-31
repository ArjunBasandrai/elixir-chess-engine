SRC = src/utils/bits.cpp src/board/board.cpp src/hashing/hash.cpp src/move.cpp src/attacks/lookup.cpp

all: __compile

debug: __compile __run

__compile:
	clang++ -Ofast -std=c++20 -o elixir.exe elixir.cpp $(SRC)

__run:
	./elixir.exe