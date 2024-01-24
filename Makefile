SRC = src/bits.cpp

all: __compile

debug: __compile __run

__compile:
	clang++ -Ofast -o elixir.exe elixir.cpp $(SRC)

__run:
	./elixir.exe