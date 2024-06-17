#pragma once

#include "../evaluate.h"
#include "../types.h"
namespace elixir::eval {
    const std::array<EvalScore, 6> material_score = {S(82, 40),   S(260, 362),  S(280, 358),
                                                     S(387, 626), S(870, 1136), S(0, 0)};

    const std::array<std::array<EvalScore, 64>, 6> psqt = {
        {{S(0, 0),     S(0, 0),     S(0, 0),    S(0, 0),    S(0, 0),    S(0, 0),    S(0, 0),
          S(0, 0),     S(56, 270),  S(71, 263), S(54, 262), S(92, 206), S(67, 208), S(48, 220),
          S(-39, 274), S(-42, 275), S(-5, 98),  S(-1, 111), S(27, 63),  S(33, 26),  S(42, 22),
          S(70, 38),   S(25, 92),   S(14, 82),  S(-29, 86), S(-16, 88), S(-15, 65), S(-6, 48),
          S(12, 48),   S(7, 50),    S(-7, 80),  S(-4, 63),  S(-39, 72), S(-27, 86), S(-24, 64),
          S(-10, 58),  S(-7, 58),   S(-6, 57),  S(-21, 79), S(-20, 56), S(-45, 67), S(-43, 82),
          S(-34, 65),  S(-30, 68),  S(-22, 68), S(-23, 63), S(-23, 76), S(-22, 53), S(-39, 72),
          S(-33, 87),  S(-31, 73),  S(-28, 69), S(-24, 81), S(0, 66),   S(-5, 78),  S(-19, 55),
          S(0, 0),     S(0, 0),     S(0, 0),    S(0, 0),    S(0, 0),    S(0, 0),    S(0, 0),
          S(0, 0)},
         {S(-166, -83), S(-138, -22),  S(-76, -5),  S(-46, -16), S(-8, -14),  S(-75, -34),
          S(-119, -18), S(-118, -103), S(-38, -25), S(-19, -7),  S(8, -6),    S(29, -7),
          S(7, -12),    S(66, -28),    S(-21, -11), S(-1, -42),  S(-10, -14), S(7, -13),
          S(-17, 2),    S(23, 5),      S(18, -9),   S(43, -24),  S(8, -25),   S(-3, -24),
          S(-33, 11),   S(-12, 7),     S(7, 12),    S(23, 20),   S(17, 15),   S(35, 14),
          S(1, 3),      S(0, -22),     S(-39, -10), S(-17, -3),  S(0, 22),    S(0, 23),
          S(10, 26),    S(7, 12),      S(6, 0),     S(-23, -18), S(-49, -15), S(-25, 0),
          S(-10, 7),    S(-1, 23),     S(10, 21),   S(-2, 2),    S(-3, -6),   S(-30, -15),
          S(-54, -18),  S(-42, -10),   S(-28, -3),  S(-13, 0),   S(-13, 0),   S(-12, -5),
          S(-20, -20),  S(-25, -11),   S(-88, -28), S(-42, -32), S(-46, -12), S(-32, -12),
          S(-26, -8),   S(-20, -21),   S(-37, -27), S(-58, -24)},
         {S(-33, 13), S(-54, 20), S(-51, 13), S(-93, 24), S(-75, 19), S(-67, 11), S(-29, 10),
          S(-57, 7),  S(-20, 0),  S(-1, 8),   S(-10, 9),  S(-25, 13), S(2, 2),    S(-7, 6),
          S(-9, 11),  S(-23, 0),  S(-8, 17),  S(10, 13),  S(9, 16),   S(24, 6),   S(13, 11),
          S(45, 15),  S(21, 12),  S(18, 15),  S(-15, 13), S(-3, 20),  S(9, 18),   S(23, 30),
          S(19, 21),  S(12, 21),  S(-2, 16),  S(-11, 13), S(-12, 10), S(-11, 20), S(-6, 26),
          S(15, 26),  S(12, 25),  S(-5, 21),  S(-7, 15),  S(-1, 0),   S(-10, 10), S(2, 18),
          S(1, 20),   S(0, 21),   S(2, 25),   S(4, 18),   S(5, 12),   S(7, 0),    S(-3, 12),
          S(4, 3),    S(9, 0),    S(-8, 13),  S(2, 9),    S(10, 5),   S(23, 7),   S(4, -4),
          S(-10, 0),  S(9, 12),   S(-7, 5),   S(-13, 10), S(-4, 8),   S(-10, 15), S(6, 0),
          S(4, -16)},
         {S(-8, 17),  S(-13, 27), S(-9, 28),  S(-8, 23),  S(2, 14),   S(9, 21),    S(-5, 24),
          S(-27, 24), S(-22, 18), S(-20, 33), S(-2, 29),  S(14, 18),  S(0, 15),    S(18, 16),
          S(4, 15),   S(-15, 13), S(-35, 16), S(-10, 20), S(-12, 15), S(-13, 11),  S(15, -2),
          S(20, 2),   S(47, 0),   S(-3, -1),  S(-39, 18), S(-27, 19), S(-27, 20),  S(-20, 13),
          S(-19, 0),  S(-10, 5),  S(-9, 8),   S(-27, 1),  S(-46, 9),  S(-47, 19),  S(-36, 12),
          S(-32, 10), S(-27, 3),  S(-38, 13), S(-19, 3),  S(-38, -3), S(-45, 3),   S(-45, 9),
          S(-36, 3),  S(-34, 5),  S(-23, -3), S(-23, 0),  S(1, -17),  S(-19, -22), S(-45, -2),
          S(-40, 6),  S(-26, 2),  S(-27, 2),  S(-19, -7), S(-17, -4), S(-10, -14), S(-43, -14),
          S(-27, 4),  S(-27, 5),  S(-20, 7),  S(-14, 0),  S(-8, -6),  S(-12, 2),   S(-17, -4),
          S(-26, -13)},
         {S(-27, 47), S(-34, 63), S(-15, 78), S(18, 63),  S(16, 66),   S(28, 61),   S(68, 7),
          S(-1, 55),  S(-1, 26),  S(-21, 55), S(-20, 86), S(-31, 107), S(-25, 122), S(9, 81),
          S(8, 67),   S(38, 61),  S(0, 29),   S(-2, 40),  S(-6, 70),   S(1, 79),    S(9, 91),
          S(50, 75),  S(54, 49),  S(56, 47),  S(-12, 39), S(-8, 49),   S(-7, 60),   S(-10, 79),
          S(-7, 92),  S(3, 84),   S(12, 80),  S(14, 70),  S(-8, 34),   S(-12, 57),  S(-8, 53),
          S(-4, 71),  S(-2, 67),  S(0, 66),   S(6, 61),   S(9, 61),    S(-6, 22),   S(-2, 37),
          S(-1, 47),  S(-1, 42),  S(1, 50),   S(4, 52),   S(16, 42),   S(10, 33),   S(-3, 19),
          S(1, 20),   S(8, 16),   S(12, 22),  S(10, 27),  S(18, 6),    S(24, -14),  S(27, -30),
          S(-6, 18),  S(-4, 15),  S(2, 18),   S(7, 29),   S(7, 16),    S(-3, 15),   S(9, -5),
          S(4, -10)},
         {S(68, -90),   S(52, -32),  S(72, -21), S(-34, 13),  S(-4, -7),   S(35, -10),  S(68, -14),
          S(142, -112), S(-62, 7),   S(-6, 38),  S(-43, 46),  S(42, 31),   S(-3, 40),   S(5, 47),
          S(17, 38),    S(-6, 9),    S(-85, 21), S(29, 41),   S(-38, 58),  S(-62, 69),  S(-26, 63),
          S(46, 51),    S(17, 46),   S(-25, 18), S(-61, 12),  S(-58, 44),  S(-79, 62),  S(-127, 76),
          S(-121, 71),  S(-87, 61),  S(-81, 47), S(-119, 22), S(-69, 0),   S(-58, 30),  S(-89, 52),
          S(-123, 70),  S(-123, 64), S(-87, 45), S(-88, 29),  S(-130, 14), S(-29, -11), S(2, 10),
          S(-56, 33),   S(-71, 47),  S(-67, 43), S(-64, 29),  S(-21, 6),   S(-41, -8),  S(56, -33),
          S(32, -5),    S(12, 6),    S(-25, 20), S(-30, 20),  S(0, 4),     S(36, -15),  S(42, -35),
          S(45, -69),   S(77, -49),  S(55, -32), S(-32, -5),  S(20, -33),  S(-9, -18),  S(59, -48),
          S(56, -79)}}};

    const std::array<EvalScore, 9> knight_mobility = {S(18, -5), S(30, -9), S(34, -3),
                                                      S(38, -6), S(40, -3), S(39, 1),
                                                      S(37, 2),  S(36, 2),  S(38, -1)};

    const std::array<EvalScore, 14> bishop_mobility = {S(-5, -56), S(2, -43), S(8, -34), S(10, -26),
                                                       S(17, -14), S(23, -2), S(28, 0),  S(32, 6),
                                                       S(33, 11),  S(34, 10), S(37, 7),  S(40, 7),
                                                       S(38, 10),  S(64, -3)};

    const std::array<EvalScore, 15> rook_mobility = {S(-11, -29), S(-2, -12), S(1, -9),  S(5, -5),
                                                     S(4, 0),     S(8, 2),    S(10, 5),  S(13, 6),
                                                     S(13, 13),   S(15, 16),  S(18, 17), S(20, 20),
                                                     S(19, 23),   S(23, 25),  S(19, 23)};

    const std::array<EvalScore, 28> queen_mobility = {
        S(-65, -9), S(-64, -107), S(-71, -53), S(-68, -38), S(-67, -33), S(-64, -28), S(-61, -19),
        S(-63, 0),  S(-61, 7),    S(-60, 10),  S(-59, 20),  S(-59, 26),  S(-58, 34),  S(-58, 40),
        S(-57, 45), S(-56, 50),   S(-54, 54),  S(-57, 64),  S(-54, 70),  S(-52, 70),  S(-43, 69),
        S(-40, 69), S(-40, 72),   S(-35, 73),  S(-16, 54),  S(35, 37),   S(57, 23),   S(147, -8)};

    const EvalScore stacked_pawn_penalty = S(0, 28);

    const EvalScore bishop_pair_bonus = S(19, 61);

    const std::array<EvalScore, 8> passed_pawn_bonus = {S(10, 20), S(-2, 2), S(2, 122), S(12, 54),
                                                        S(-5, 26), S(-5, 0), S(-2, -2), S(10, 20)};

    const std::array<EvalScore, 8> rook_open_file_bonus = {S(28, 8),  S(24, 3),  S(21, 11),
                                                           S(22, 11), S(24, 16), S(40, 1),
                                                           S(58, -1), S(103, -8)};

    const std::array<EvalScore, 8> rook_semi_open_file_bonus = {S(1, 39), S(7, 12), S(5, 15),
                                                                S(11, 7), S(7, 8),  S(14, 0),
                                                                S(25, 0), S(26, 19)};

    const std::array<EvalScore, 8> isolated_pawn_penalty = {S(0, -9),  S(2, 9), S(9, 3),  S(8, 10),
                                                            S(10, 11), S(4, 2), S(0, 10), S(2, -7)};

    const std::array<EvalScore, 64> knight_outpost_bonus = {
        S(7, 0),    S(3, -1),  S(0, 0),    S(1, 0),    S(-1, 0),   S(4, 0),   S(4, -1),
        S(2, 1),    S(0, 0),   S(0, 0),    S(1, 0),    S(0, -1),   S(1, 0),   S(0, 0),
        S(2, -1),   S(0, -1),  S(-8, 0),   S(11, 9),   S(59, 10),  S(24, 9),  S(66, 8),
        S(50, 13),  S(33, 11), S(15, -1),  S(19, -15), S(24, 2),   S(29, 11), S(36, 1),
        S(41, 6),   S(53, 0),  S(64, -2),  S(37, 6),   S(20, 9),   S(18, 9),  S(28, -1),
        S(25, 0),   S(38, -3), S(42, 1),   S(48, -1),  S(24, 13),  S(0, -1),  S(-3, 4),
        S(4, 5),    S(4, -7),  S(7, -9),   S(1, 8),    S(12, -5),  S(7, -3),  S(-3, -7),
        S(-12, 8),  S(-6, 10), S(-10, -3), S(-10, -5), S(-12, -2), S(-21, 6), S(-7, -9),
        S(-9, 1),   S(1, -2),  S(-45, 11), S(-24, 9),  S(-19, -3), S(-25, 6), S(-18, 9),
        S(-26, -24)};

    const std::array<EvalScore, 8> supported_pawn_bonus = {S(0, 0),   S(117, 145), S(18, 54),
                                                           S(12, 15), S(14, 5),    S(19, 10),
                                                           S(52, 73), S(0, 0)};

    const std::array<EvalScore, 8> pawn_duo_bonus = {S(0, 0),  S(29, 123), S(61, 94), S(21, 30),
                                                     S(11, 9), S(9, 2),    S(4, 0),   S(0, 0)};

    const std::array<EvalScore, 8> king_open_file_penalty = {S(64, 26),  S(112, 6), S(88, 4),
                                                             S(75, 2),   S(65, -2), S(93, -10),
                                                             S(97, -13), S(59, -3)};

    const std::array<EvalScore, 8> king_semi_open_file_penalty = {S(26, -56), S(66, -30),
                                                                  S(44, -26), S(19, -14),
                                                                  S(13, -6),  S(33, -15),
                                                                  S(49, -28), S(34, -41)};


}