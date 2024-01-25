#include <iostream>

#include "src/defs.h"
#include "src/bits.h"

int main() {
    U64 b = 0x8000000000000001ULL;
    print_bitboard(b);
    std::cout << "Number of bits: " << count_bits(b) << "\n";
    std::cout << "MSB index: " << msb_index(b) << "\n";
    std::cout << "LSB index: " << lsb_index(b) << "\n";
    return 0;
}