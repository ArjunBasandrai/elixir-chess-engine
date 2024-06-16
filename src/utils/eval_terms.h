#pragma once

#include "../evaluate.h"
#include "../types.h"
namespace elixir::eval {
    const std::array<EvalScore, 6> material_score = {S(113, 69),  S(283, 385),  S(300, 377),
                                                     S(424, 676), S(990, 1214), S(0, 0)};

    const std::array<std::array<EvalScore, 64>, 6> psqt = {
        {{S(0, 0),     S(0, 0),     S(0, 0),     S(0, 0),     S(0, 0),    S(0, 0),    S(0, 0),
          S(0, 0),     S(57, 293),  S(83, 275),  S(60, 276),  S(94, 216), S(72, 212), S(58, 228),
          S(-19, 279), S(-43, 293), S(-28, 187), S(-14, 197), S(23, 160), S(30, 132), S(33, 123),
          S(58, 113),  S(30, 165),  S(-7, 154),  S(-45, 96),  S(-22, 88), S(-19, 68), S(-12, 51),
          S(7, 46),    S(4, 54),    S(3, 74),    S(-15, 66),  S(-57, 69), S(-31, 70), S(-29, 50),
          S(-13, 40),  S(-11, 41),  S(-9, 47),   S(-9, 59),   S(-29, 45), S(-55, 65), S(-35, 68),
          S(-32, 52),  S(-27, 55),  S(-13, 53),  S(-14, 53),  S(6, 56),   S(-22, 43), S(-57, 70),
          S(-35, 72),  S(-41, 62),  S(-39, 69),  S(-26, 71),  S(-3, 58),  S(15, 55),  S(-33, 45),
          S(0, 0),     S(0, 0),     S(0, 0),     S(0, 0),     S(0, 0),    S(0, 0),    S(0, 0),
          S(0, 0)},
         {S(-185, -81), S(-146, -22),  S(-81, -8),   S(-53, -17), S(-5, -17),  S(-88, -34),
          S(-129, -19), S(-128, -108), S(-42, -26),  S(-22, -8),  S(8, -7),    S(29, -8),
          S(8, -17),    S(71, -31),    S(-24, -13),  S(-1, -45),  S(-25, -15), S(16, -4),
          S(37, 15),    S(47, 16),     S(87, -1),    S(91, -8),   S(41, -16),  S(9, -28),
          S(-23, -2),   S(-6, 13),     S(20, 26),    S(44, 27),   S(22, 29),   S(50, 22),
          S(6, 10),     S(15, -14),    S(-36, -2),   S(-20, 4),   S(3, 28),    S(5, 28),
          S(16, 31),    S(8, 19),      S(3, 4),      S(-23, -11), S(-56, -18), S(-30, -2),
          S(-13, 6),    S(-2, 21),     S(11, 19),    S(-3, 0),    S(-5, -8),   S(-34, -17),
          S(-63, -25),  S(-51, -11),   S(-34, -2),   S(-17, -2),  S(-16, -2),  S(-15, -7),
          S(-31, -21),  S(-33, -17),   S(-104, -32), S(-47, -38), S(-57, -15), S(-41, -12),
          S(-35, -11),  S(-26, -23),   S(-43, -29),  S(-72, -40)},
         {S(-28, 18), S(-55, 26), S(-52, 19), S(-98, 32), S(-86, 28), S(-62, 15), S(-29, 13),
          S(-61, 14), S(-17, 3),  S(0, 12),   S(-8, 14),  S(-25, 16), S(5, 5),    S(-2, 8),
          S(-3, 16),  S(-8, 1),   S(-6, 24),  S(14, 17),  S(11, 20),  S(28, 9),   S(16, 14),
          S(52, 18),  S(34, 14),  S(23, 20),  S(-13, 18), S(-2, 25),  S(14, 21),  S(27, 34),
          S(23, 25),  S(18, 25),  S(-1, 20),  S(-10, 16), S(-12, 13), S(-9, 25),  S(-5, 31),
          S(21, 28),  S(16, 28),  S(-3, 25),  S(-7, 20),  S(0, 4),    S(-6, 16),  S(4, 22),
          S(6, 25),   S(3, 26),   S(6, 30),   S(8, 23),   S(8, 16),   S(11, 5),   S(0, 16),
          S(8, 8),    S(14, 2),   S(-5, 19),  S(6, 14),   S(16, 9),   S(26, 14),  S(7, 0),
          S(-9, 4),   S(13, 17),  S(-4, 10),  S(-14, 16), S(-6, 16),  S(-9, 23),  S(12, 1),
          S(5, -10)},
         {S(-6, 22),  S(-17, 34), S(-15, 36), S(-14, 30), S(-1, 20),  S(26, 22),   S(17, 20),
          S(11, 14),  S(-22, 25), S(-24, 41), S(-2, 35),  S(15, 22),  S(-6, 22),   S(17, 26),
          S(4, 21),   S(10, 8),   S(-41, 23), S(-13, 28), S(-15, 21), S(-19, 17),  S(9, 2),
          S(20, 8),   S(58, 0),   S(6, -4),   S(-43, 22), S(-31, 23), S(-32, 24),  S(-27, 18),
          S(-26, 1),  S(-12, 7),  S(-13, 6),  S(-24, -1), S(-51, 13), S(-55, 23),  S(-42, 16),
          S(-38, 13), S(-36, 7),  S(-44, 17), S(-26, 5),  S(-41, -5), S(-51, 6),   S(-51, 13),
          S(-40, 5),  S(-38, 7),  S(-29, -1), S(-27, 2),  S(-2, -18), S(-22, -24), S(-50, 0),
          S(-45, 10), S(-29, 5),  S(-31, 6),  S(-24, -4), S(-18, 0),  S(-18, -10), S(-48, -16),
          S(-29, 7),  S(-30, 9),  S(-21, 10), S(-16, 3),  S(-12, -5), S(-12, 5),   S(-19, -1),
          S(-31, -9)},
         {S(-25, 51), S(-39, 75), S(-12, 88), S(19, 73),  S(24, 71),   S(33, 69),   S(65, 17),
          S(6, 59),   S(0, 34),   S(-25, 64), S(-23, 98), S(-34, 120), S(-29, 138), S(10, 94),
          S(0, 79),   S(49, 64),  S(3, 34),   S(-4, 46),  S(-8, 80),   S(0, 88),    S(10, 101),
          S(58, 81),  S(68, 46),  S(67, 46),  S(-13, 42), S(-8, 52),   S(-8, 64),   S(-10, 83),
          S(-9, 98),  S(5, 90),   S(13, 84),  S(19, 72),  S(-7, 37),   S(-12, 61),  S(-8, 57),
          S(-2, 75),  S(-1, 71),  S(-1, 70),  S(7, 65),   S(12, 63),   S(-6, 29),   S(0, 39),
          S(0, 50),   S(0, 43),   S(4, 51),   S(7, 55),   S(19, 43),   S(13, 35),   S(-1, 21),
          S(3, 21),   S(11, 17),  S(16, 23),  S(13, 31),  S(24, 6),    S(30, -19),  S(35, -38),
          S(-3, 21),  S(-3, 19),  S(5, 18),   S(11, 32),  S(10, 17),   S(-2, 20),   S(20, -6),
          S(8, -9)},
         {S(69, -106), S(46, -50),   S(73, -36),  S(-29, 4),   S(8, -9),    S(44, -4),
          S(68, -9),   S(139, -102), S(-59, -4),  S(8, 26),    S(-29, 37),  S(45, 25),
          S(8, 44),    S(24, 55),    S(18, 51),   S(-13, 17),  S(-77, 10),  S(46, 32),
          S(-38, 54),  S(-48, 64),   S(-27, 67),  S(53, 59),   S(29, 57),   S(-26, 26),
          S(-53, 0),   S(-66, 39),   S(-80, 60),  S(-130, 73), S(-131, 75), S(-92, 68),
          S(-82, 57),  S(-115, 29),  S(-61, -8),  S(-69, 26),  S(-107, 53), S(-146, 71),
          S(-146, 70), S(-104, 56),  S(-107, 42), S(-140, 26), S(-28, -15), S(-5, 9),
          S(-69, 34),  S(-86, 47),   S(-79, 46),  S(-76, 37),  S(-31, 17),  S(-50, 2),
          S(68, -37),  S(22, -7),    S(7, 5),     S(-30, 17),  S(-33, 21),  S(-13, 10),
          S(33, -8),   S(42, -28),   S(58, -76),  S(87, -54),  S(60, -35),  S(-43, -13),
          S(19, -35),  S(-13, -17),  S(62, -47),  S(63, -77)}}};

    const std::array<EvalScore, 9> knight_mobility = {S(24, -1), S(39, 0),  S(46, 7),
                                                      S(49, 3),  S(52, 7),  S(51, 11),
                                                      S(49, 12), S(48, 13), S(50, 9)};

    const std::array<EvalScore, 14> bishop_mobility = {S(0, -60), S(7, -40), S(15, -33), S(18, -23),
                                                       S(26, -9), S(33, 4),  S(39, 7),   S(43, 14),
                                                       S(43, 22), S(45, 20), S(49, 19),  S(52, 19),
                                                       S(50, 22), S(81, 5)};

    const std::array<EvalScore, 15> rook_mobility = {S(-8, -26), S(1, -7),  S(5, -4),  S(8, 0),
                                                     S(7, 4),    S(12, 7),  S(14, 9),  S(18, 11),
                                                     S(18, 20),  S(21, 22), S(24, 23), S(26, 26),
                                                     S(25, 31),  S(29, 33), S(26, 31)};

    const std::array<EvalScore, 28> queen_mobility = {
        S(-66, -3), S(-64, -94), S(-72, -57), S(-70, -40), S(-68, -37), S(-65, -30), S(-61, -22),
        S(-63, 0),  S(-61, 6),   S(-59, 10),  S(-58, 22),  S(-58, 29),  S(-58, 38),  S(-57, 44),
        S(-55, 50), S(-55, 57),  S(-52, 61),  S(-55, 73),  S(-52, 79),  S(-50, 80),  S(-40, 79),
        S(-37, 82), S(-35, 83),  S(-26, 81),  S(-7, 65),   S(46, 49),   S(69, 32),   S(160, -2)};

    const EvalScore stacked_pawn_penalty = S(22, 38);

    const EvalScore bishop_pair_bonus = S(21, 69);

    const std::array<EvalScore, 8> passed_pawn_bonus = {S(10, 20),   S(-10, -13), S(-12, 8),
                                                        S(-4, 31),   S(-3, 30),   S(-12, 10),
                                                        S(-14, -16), S(10, 20)};

    const std::array<EvalScore, 8> rook_open_file_bonus = {S(31, 7),  S(29, 1),  S(25, 11),
                                                           S(27, 12), S(33, 15), S(47, -1),
                                                           S(69, -3), S(114, -7)};

    const std::array<EvalScore, 8> rook_semi_open_file_bonus = {S(4, 40),  S(12, 12), S(7, 17),
                                                                S(17, 7),  S(14, 7),  S(17, 0),
                                                                S(40, -1), S(35, 19)};

}