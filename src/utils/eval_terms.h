#pragma once

#include "../evaluate.h"
#include "../types.h"
namespace elixir::eval {
    const std::array<EvalScore, 6> material_score = {S(82, 94),  S(337, 281),  S(365, 297),
                                                     S(477, 512), S(1025, 936), S(0, 0)};
    const std::array<std::array<EvalScore, 64>, 6> psqt                  = {{
                          {S(0, 0),     S(0, 0),     S(0, 0),     S(0, 0),     S(0, 0),    S(0, 0),    S(0, 0),
                           S(0, 0),     S(41, 285),  S(68, 276),  S(49, 270),  S(75, 217), S(65, 211), S(55, 229),
                           S(-13, 280), S(-55, 290), S(-36, 182), S(-13, 191), S(20, 156), S(23, 129), S(29, 119),
                           S(54, 113),  S(32, 160),  S(-19, 154), S(-47, 93),  S(-20, 86), S(-19, 67), S(-12, 50),
                           S(6, 47),    S(1, 55),    S(5, 73),    S(-25, 67),  S(-58, 66), S(-29, 69), S(-27, 49),
                           S(-10, 39),  S(-8, 41),   S(-11, 49),  S(-9, 59),   S(-44, 48), S(-53, 61), S(-33, 68),
                           S(-27, 51),  S(-24, 55),  S(-8, 54),   S(-13, 54),  S(8, 57),   S(-26, 44), S(-52, 68),
                           S(-28, 71),  S(-37, 64),  S(-32, 71),  S(-19, 71),  S(0, 61),   S(21, 56),  S(-34, 46),
                           S(0, 0),     S(0, 0),     S(0, 0),     S(0, 0),     S(0, 0),    S(0, 0),    S(0, 0),
                           S(0, 0)},
                          {S(-200, -72), S(-146, -26),  S(-80, -10),  S(-54, -17), S(-4, -19),  S(-94, -34),
                           S(-131, -22), S(-132, -110), S(-59, -23),  S(-32, -7),  S(22, -13),  S(22, -7),
                           S(21, -20),   S(73, -32),    S(-20, -16),  S(-4, -43),  S(-27, -16), S(15, -6),
                           S(34, 16),    S(49, 15),     S(90, -4),    S(98, -10),  S(46, -18),  S(9, -27),
                           S(-21, -4),   S(-4, 12),     S(18, 26),    S(47, 28),   S(22, 29),   S(51, 22),
                           S(4, 11),     S(17, -16),    S(-34, -3),   S(-19, 4),   S(4, 27),    S(7, 28),
                           S(18, 31),    S(12, 17),     S(4, 2),      S(-25, -10), S(-52, -20), S(-28, -3),
                           S(-6, 2),     S(1, 20),      S(14, 17),    S(1, -2),    S(-3, -11),  S(-33, -20),
                           S(-58, -27),  S(-48, -14),   S(-31, -5),   S(-11, -3),  S(-11, -4),  S(-11, -9),
                           S(-29, -23),  S(-29, -21),   S(-103, -36), S(-37, -36), S(-53, -16), S(-33, -13),
                           S(-30, -13),  S(-24, -23),   S(-35, -33),  S(-75, -39)},
                          {S(-25, 15), S(-58, 23), S(-52, 17), S(-98, 32), S(-92, 28), S(-57, 16), S(-28, 12),
                           S(-63, 16), S(-26, 8),  S(-1, 12),  S(-15, 16), S(-25, 15), S(1, 7),    S(9, 6),
                           S(-4, 17),  S(1, -3),   S(-8, 24),  S(15, 17),  S(20, 17),  S(28, 11),  S(21, 14),
                           S(53, 19),  S(41, 13),  S(29, 17),  S(-16, 21), S(-4, 27),  S(14, 21),  S(27, 33),
                           S(23, 25),  S(16, 24),  S(0, 19),   S(-9, 15),  S(-12, 14), S(-6, 23),  S(-1, 29),
                           S(24, 27),  S(20, 26),  S(-1, 25),  S(-5, 18),  S(-2, 5),   S(0, 14),   S(7, 21),
                           S(10, 24),  S(7, 25),   S(10, 28),  S(13, 21),  S(7, 15),   S(13, 5),   S(4, 14),
                           S(14, 7),   S(16, 1),   S(0, 17),   S(10, 14),  S(20, 8),   S(32, 12),  S(8, 0),
                           S(-6, 2),   S(17, 14),  S(3, 11),   S(-6, 16),  S(-2, 17),  S(-9, 24),  S(14, 0),
                           S(1, -6)},
                          {S(-4, 22),  S(-18, 28), S(-17, 35), S(-16, 30), S(3, 22),   S(32, 12),   S(22, 13),
                           S(37, 10),  S(-24, 24), S(-33, 37), S(-7, 37),  S(15, 25),  S(0, 25),    S(32, 14),
                           S(31, 9),   S(60, -3),  S(-43, 23), S(-19, 23), S(-27, 24), S(-22, 20),  S(9, 6),
                           S(21, 1),   S(74, -8),  S(45, -11), S(-52, 24), S(-39, 18), S(-40, 27),  S(-33, 21),
                           S(-31, 9),  S(-15, 2),  S(1, -1),   S(0, -5),   S(-61, 18), S(-62, 20),  S(-54, 22),
                           S(-44, 19), S(-40, 14), S(-47, 13), S(-19, 0),  S(-36, -1), S(-60, 13),  S(-55, 11),
                           S(-46, 10), S(-39, 11), S(-31, 6),  S(-30, 0),  S(0, -20),  S(-22, -16), S(-59, 7),
                           S(-49, 9),  S(-32, 9),  S(-30, 10), S(-23, 1),  S(-18, -3), S(-2, -13),  S(-41, -5),
                           S(-36, 8),  S(-33, 8),  S(-20, 11), S(-12, 5),  S(-7, 0),   S(-13, 3),   S(-9, -4),
                           S(-28, -10)},
                          {S(-23, 48),  S(-40, 75), S(-12, 86), S(19, 74),  S(25, 71),   S(35, 70),   S(62, 17),
                           S(6, 63),    S(-5, 39),  S(-29, 61), S(-28, 97), S(-42, 123), S(-35, 137), S(18, 96),
                           S(3, 82),    S(66, 52),  S(0, 32),   S(-7, 45),  S(-5, 73),   S(1, 83),    S(12, 104),
                           S(58, 79),   S(76, 43),  S(74, 43),  S(-16, 41), S(-12, 53),  S(-11, 63),  S(-14, 84),
                           S(-11, 100), S(1, 97),   S(10, 88),  S(20, 73),  S(-10, 42),  S(-13, 60),  S(-9, 58),
                           S(-6, 83),   S(-2, 74),  S(-1, 71),  S(6, 66),   S(11, 64),   S(-6, 26),   S(2, 34),
                           S(0, 53),    S(3, 42),   S(5, 53),   S(11, 52),  S(20, 44),   S(14, 35),   S(1, 24),
                           S(4, 25),    S(14, 20),  S(21, 21),  S(18, 28),  S(31, 0),    S(34, -23),  S(42, -37),
                           S(1, 22),    S(2, 17),   S(14, 14),  S(22, 25),  S(18, 14),   S(0, 18),    S(20, -6),
                           S(13, -8)},
                          {S(71, -100), S(48, -42),  S(75, -32),  S(-27, 4),   S(11, -5),   S(46, -1),   S(68, -8),
                           S(141, -91), S(-57, -3),  S(13, 23),   S(-25, 32),  S(45, 21),   S(10, 37),   S(27, 50),
                           S(17, 47),   S(-15, 17),  S(-73, 8),   S(52, 28),   S(-35, 49),  S(-42, 56),  S(-27, 58),
                           S(54, 52),   S(32, 53),   S(-27, 24),  S(-51, -2),  S(-63, 34),  S(-73, 55),  S(-121, 68),
                           S(-125, 70), S(-87, 64),  S(-75, 53),  S(-110, 27), S(-58, -11), S(-64, 23),  S(-99, 50),
                           S(-143, 69), S(-142, 69), S(-106, 56), S(-102, 41), S(-138, 25), S(-27, -16), S(-10, 10),
                           S(-75, 35),  S(-91, 49),  S(-86, 49),  S(-80, 40),  S(-35, 20),  S(-53, 5),   S(61, -34),
                           S(15, -4),   S(-3, 10),   S(-42, 23),  S(-43, 26),  S(-21, 16),  S(30, -4),   S(40, -24),
                           S(49, -71),  S(86, -52),  S(61, -31),  S(-48, -9),  S(19, -31),  S(-17, -12), S(64, -44),
                           S(61, -75)}}};
    const std::array<EvalScore, 9> knight_mobility = {S(25, -4), S(44, -3), S(54, 2), S(57, 0), S(61, 2),
                                            S(60, 7),  S(59, 8),  S(58, 8), S(59, 4)};
    const std::array<EvalScore, 14> bishop_mobility = {S(6, -66),  S(14, -43), S(23, -37), S(25, -26),
                                            S(33, -13), S(40, 0),   S(46, 4),   S(49, 11),
                                            S(51, 18),  S(52, 17),  S(57, 15),  S(60, 16),
                                            S(61, 20),  S(86, 5)};
    const std::array<EvalScore, 15> rook_mobility  = {S(-15, -28), S(-4, -11), S(0, -7),  S(4, -4),  S(4, 2),
                                            S(13, 4),    S(17, 7),   S(26, 9),  S(30, 18), S(37, 21),
                                            S(43, 22),   S(47, 27),  S(49, 33), S(56, 36), S(57, 34)};
    const std::array<EvalScore, 28> queen_mobility   = {S(-72, -3),  S(-70, -90), S(-73, -57), S(-70, -41),
                                            S(-67, -41), S(-63, -32), S(-59, -25), S(-61, -4),
                                            S(-59, 6),   S(-56, 9),   S(-55, 21),  S(-55, 28),
                                            S(-54, 37),  S(-52, 43),  S(-50, 48),  S(-49, 53),
                                            S(-46, 58),  S(-49, 70),  S(-44, 75),  S(-43, 76),
                                            S(-36, 78),  S(-32, 80),  S(-30, 80),  S(-18, 76),
                                            S(-7, 65),   S(46, 49),   S(71, 34),   S(161, -4)};
    const EvalScore stacked_pawn_penalty = S(20, 35);
    const EvalScore bishop_pair_bonus    = S(25, 66);
    const std::array<EvalScore, 8> passed_pawn_bonus = {S(10, 20), S(-9, -7),  S(-13, 16), S(-4, 36),
                                            S(-3, 37), S(-16, 17), S(-16, -9), S(10, 20)};
}