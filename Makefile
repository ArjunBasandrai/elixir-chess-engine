SRC := $(wildcard src/*.cpp) $(wildcard src/**/*.cpp) $(wildcard src/**/**/*.cpp)

CXX = g++
EXE = elixir

SUFFIX = 
ifeq ($(OS),Windows_NT)
	SUFFIX = .exe
endif

EXE_NAME = $(EXE)$(SUFFIX)

AVX2FLAGS    = -DUSE_AVX2 -DUSE_SIMD -mavx2 -mbmi
BMI2FLAGS    = -DUSE_AVX2 -DUSE_SIMD -mavx2 -mbmi -mbmi2
AVX512FLAGS  = -DUSE_AVX512 -DUSE_SIMD -mavx512f -mavx512bw

CXXFLAGS = -flto=auto -pthread -fno-exceptions -fno-rtti -fwhole-program -fomit-frame-pointer -ffast-math -funroll-all-loops -fno-align-functions 

ARCH_DETECTED =
PROPERTIES = $(shell echo | $(CXX) -march=native -E -dM -)
ifneq ($(findstring __AVX512F__, $(PROPERTIES)),)
	ifneq ($(findstring __AVX512BW__, $(PROPERTIES)),)
		ARCH_DETECTED = AVX512
	endif
endif
ifeq ($(ARCH_DETECTED),)
	ifneq ($(findstring __BMI2__, $(PROPERTIES)),)
		ARCH_DETECTED = BMI2
	endif
endif
ifeq ($(ARCH_DETECTED),)
	ifneq ($(findstring __AVX2__, $(PROPERTIES)),)
		ARCH_DETECTED = AVX2
	endif
endif

ifeq ($(ARCH_DETECTED), AVX512)
	CXXFLAGS += $(AVX512FLAGS)
endif
ifeq ($(ARCH_DETECTED), BMI2)
	CXXFLAGS += $(BMI2FLAGS)
endif
ifeq ($(ARCH_DETECTED), AVX2)
	CXXFLAGS += $(AVX2FLAGS)
endif

INCLUDE_PATHS = -Isrc/ -Isrc/core/
CXXFLAGS += -std=c++20 $(INCLUDE_PATHS)

all: __compile

debug: __debug_compile __run

tune: __tune_compile

texel: __texel_compile __texel_run

__compile:
	$(CXX) -Ofast -march=native -DNDEBUG -DEVALFILE=\"nnue.bin\" $(CXXFLAGS) -o $(EXE_NAME) elixir.cpp $(SRC)

__tune_compile:
	$(CXX) -Ofast -march=native -DNDEBUG -DUSE_TUNE -DEVALFILE=\"nnue.bin\" $(CXXFLAGS) -o $(EXE_NAME) elixir.cpp $(SRC)

__texel_compile:
	$(CXX) -Ofast -march=native -DNDEBUG -DTEXEL -DEVALFILE=\"nnue.bin\" $(CXXFLAGS) -o $(EXE_NAME) elixir.cpp $(SRC)

__debug_compile:
	$(CXX) -Og -g -DEVALFILE=\"nnue.bin\" $(CXXFLAGS) -o $(EXE_NAME) elixir.cpp $(SRC)

__run:
	./$(EXE_NAME)

__texel_run:
	./$(EXE_NAME) texel