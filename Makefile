SRC = src/utils/bits.cpp src/board/board.cpp

all: __compile

debug: __compile __run

__compile:
	clang++ -Ofast -std=c++20 -o elixir.exe elixir.cpp $(SRC)

__run:
	./elixir.exe