#pragma once

#include "../defs.h"
#include "../types.h"

namespace elixir::masks {
    constexpr Bitboard isolated_pawn_masks[8] = {0x202020202020202,  0x505050505050505,
                                                 0xA0A0A0A0A0A0A0A,  0x1414141414141414,
                                                 0x2828282828282828, 0x5050505050505050,
                                                 0xA0A0A0A0A0A0A0A0, 0x4040404040404040};

    constexpr Bitboard passed_pawn_masks[2][64] = {{0x303030303030300ULL,
                                                    0x707070707070700ULL,
                                                    0xe0e0e0e0e0e0e00ULL,
                                                    0x1c1c1c1c1c1c1c00ULL,
                                                    0x3838383838383800ULL,
                                                    0x7070707070707000ULL,
                                                    0xe0e0e0e0e0e0e000ULL,
                                                    0xc0c0c0c0c0c0c000ULL,
                                                    0x303030303030000ULL,
                                                    0x707070707070000ULL,
                                                    0xe0e0e0e0e0e0000ULL,
                                                    0x1c1c1c1c1c1c0000ULL,
                                                    0x3838383838380000ULL,
                                                    0x7070707070700000ULL,
                                                    0xe0e0e0e0e0e00000ULL,
                                                    0xc0c0c0c0c0c00000ULL,
                                                    0x303030303000000ULL,
                                                    0x707070707000000ULL,
                                                    0xe0e0e0e0e000000ULL,
                                                    0x1c1c1c1c1c000000ULL,
                                                    0x3838383838000000ULL,
                                                    0x7070707070000000ULL,
                                                    0xe0e0e0e0e0000000ULL,
                                                    0xc0c0c0c0c0000000ULL,
                                                    0x303030300000000ULL,
                                                    0x707070700000000ULL,
                                                    0xe0e0e0e00000000ULL,
                                                    0x1c1c1c1c00000000ULL,
                                                    0x3838383800000000ULL,
                                                    0x7070707000000000ULL,
                                                    0xe0e0e0e000000000ULL,
                                                    0xc0c0c0c000000000ULL,
                                                    0x303030000000000ULL,
                                                    0x707070000000000ULL,
                                                    0xe0e0e0000000000ULL,
                                                    0x1c1c1c0000000000ULL,
                                                    0x3838380000000000ULL,
                                                    0x7070700000000000ULL,
                                                    0xe0e0e00000000000ULL,
                                                    0xc0c0c00000000000ULL,
                                                    0x303000000000000ULL,
                                                    0x707000000000000ULL,
                                                    0xe0e000000000000ULL,
                                                    0x1c1c000000000000ULL,
                                                    0x3838000000000000ULL,
                                                    0x7070000000000000ULL,
                                                    0xe0e0000000000000ULL,
                                                    0xc0c0000000000000ULL,
                                                    0x300000000000000ULL,
                                                    0x700000000000000ULL,
                                                    0xe00000000000000ULL,
                                                    0x1c00000000000000ULL,
                                                    0x3800000000000000ULL,
                                                    0x7000000000000000ULL,
                                                    0xe000000000000000ULL,
                                                    0xc000000000000000ULL,
                                                    0x0ULL,
                                                    0x0ULL,
                                                    0x0ULL,
                                                    0x0ULL,
                                                    0x0ULL,
                                                    0x0ULL,
                                                    0x0ULL,
                                                    0x0ULL},
                                                   {0ULL,
                                                    0ULL,
                                                    0ULL,
                                                    0ULL,
                                                    0ULL,
                                                    0ULL,
                                                    0ULL,
                                                    0ULL,
                                                    3ULL,
                                                    7ULL,
                                                    14ULL,
                                                    28ULL,
                                                    56ULL,
                                                    112ULL,
                                                    224ULL,
                                                    192ULL,
                                                    771ULL,
                                                    1799ULL,
                                                    3598ULL,
                                                    7196ULL,
                                                    14392ULL,
                                                    28784ULL,
                                                    57568ULL,
                                                    49344ULL,
                                                    197379ULL,
                                                    460551ULL,
                                                    921102ULL,
                                                    1842204ULL,
                                                    3684408ULL,
                                                    7368816ULL,
                                                    14737632ULL,
                                                    12632256ULL,
                                                    50529027ULL,
                                                    117901063ULL,
                                                    235802126ULL,
                                                    471604252ULL,
                                                    943208504ULL,
                                                    1886417008ULL,
                                                    3772834016ULL,
                                                    3233857728ULL,
                                                    12935430915ULL,
                                                    30182672135ULL,
                                                    60365344270ULL,
                                                    120730688540ULL,
                                                    241461377080ULL,
                                                    482922754160ULL,
                                                    965845508320ULL,
                                                    827867578560ULL,
                                                    3311470314243ULL,
                                                    7726764066567ULL,
                                                    15453528133134ULL,
                                                    30907056266268ULL,
                                                    61814112532536ULL,
                                                    123628225065072ULL,
                                                    247256450130144ULL,
                                                    211934100111552ULL,
                                                    847736400446211ULL,
                                                    1978051601041159ULL,
                                                    3956103202082318ULL,
                                                    7912206404164636ULL,
                                                    15824412808329272ULL,
                                                    31648825616658544ULL,
                                                    63297651233317088ULL,
                                                    54255129628557504ULL}};
}