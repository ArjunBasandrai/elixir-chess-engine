#include <iostream>

#include "src/types.h"
#include "src/defs.h"
#include "src/bits.h"

using namespace elixir;

int main() {
    U64 b = 0x8000000000000002ULL;
    print_bitboard(b);
    std::cout << "Number of bits: " << bits::count_bits(b) << "\n";
    std::cout << "MSB index: " << bits::msb_index(b) << "\n";
    std::cout << "LSB index: " << bits::lsb_index(b) << "\n";
    std::cout << "LSB index: " << bits::pop_bit(b) << "\n";
    print_bitboard(b);
    return 0;
}