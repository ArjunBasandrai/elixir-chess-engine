#pragma once

#include "../evaluate.h"
#include "../types.h"
namespace elixir::eval {
    const std::array<EvalScore, 6> material_score = {S(70, 33),   S(243, 339),  S(261, 337),
                                                     S(354, 577), S(794, 1017), S(0, 0)};

    const std::array<std::array<EvalScore, 64>, 6> psqt = {
        {{S(0, 0),    S(0, 0),     S(0, 0),    S(0, 0),    S(0, 0),    S(0, 0),    S(0, 0),
          S(0, 0),    S(43, 251),  S(56, 248), S(32, 256), S(68, 220), S(65, 222), S(54, 231),
          S(8, 267),  S(-21, 265), S(-7, 83),  S(-5, 97),  S(19, 60),  S(22, 36),  S(37, 33),
          S(51, 46),  S(22, 85),   S(-9, 79),  S(-25, 78), S(-12, 82), S(-9, 61),  S(-4, 49),
          S(17, 47),  S(1, 52),    S(0, 75),   S(-15, 62), S(-34, 69), S(-22, 82), S(-17, 61),
          S(-6, 56),  S(-1, 54),   S(-7, 56),  S(-9, 73),  S(-26, 55), S(-40, 65), S(-37, 79),
          S(-27, 62), S(-24, 64),  S(-17, 62), S(-20, 59), S(-16, 69), S(-29, 52), S(-34, 69),
          S(-28, 83), S(-26, 68),  S(-31, 65), S(-20, 72), S(-11, 61), S(-4, 70),  S(-28, 54),
          S(0, 0),    S(0, 0),     S(0, 0),    S(0, 0),    S(0, 0),    S(0, 0),    S(0, 0),
          S(0, 0)},
         {S(-159, -78), S(-129, -22), S(-73, -6),  S(-43, -18), S(-11, -15), S(-71, -33),
          S(-114, -19), S(-114, -97), S(-38, -25), S(-17, -8),  S(7, -8),    S(26, -8),
          S(1, -12),    S(58, -27),   S(-17, -12), S(-2, -41),  S(-2, -16),  S(-1, -21),
          S(-33, 4),    S(3, -2),     S(-8, -1),   S(13, -22),  S(3, -27),   S(-13, -20),
          S(-30, 9),    S(-13, 3),    S(5, 8),     S(16, 16),   S(10, 13),   S(28, 11),
          S(-2, 2),     S(-4, -21),   S(-36, -12), S(-16, -7),  S(-1, 17),   S(-3, 17),
          S(4, 20),     S(1, 8),      S(4, -3),    S(-24, -20), S(-45, -17), S(-24, -3),
          S(-11, 3),    S(-2, 17),    S(8, 14),    S(-5, 0),    S(-5, -8),   S(-29, -16),
          S(-49, -20),  S(-40, -12),  S(-25, -6),  S(-14, -2),  S(-14, -3),  S(-11, -7),
          S(-19, -22),  S(-22, -12),  S(-78, -28), S(-41, -31), S(-42, -14), S(-31, -13),
          S(-25, -10),  S(-20, -21),  S(-38, -25), S(-50, -25)},
         {S(-33, 10), S(-50, 13), S(-50, 8), S(-84, 16), S(-74, 14), S(-66, 4),  S(-28, 4),
          S(-56, 1),  S(-19, -4), S(-1, 4),  S(-9, 4),   S(-24, 9),  S(-2, -1),  S(-4, 1),
          S(-9, 6),   S(-27, -1), S(-9, 11), S(7, 8),    S(6, 12),   S(17, 3),   S(11, 6),
          S(33, 12),  S(14, 9),   S(7, 10),  S(-17, 7),  S(-5, 15),  S(5, 13),   S(18, 25),
          S(11, 17),  S(5, 17),   S(-7, 12), S(-16, 8),  S(-13, 6),  S(-13, 14), S(-8, 21),
          S(9, 21),   S(6, 19),   S(-8, 16), S(-9, 11),  S(-4, -4),  S(-13, 5),  S(0, 13),
          S(-1, 15),  S(-3, 16),  S(0, 19),  S(1, 13),   S(2, 8),    S(3, -3),   S(-5, 8),
          S(0, 0),    S(6, -3),   S(-10, 8), S(0, 6),    S(8, 1),    S(19, 2),   S(2, -7),
          S(-13, -4), S(5, 6),    S(-11, 1), S(-14, 5),  S(-6, 3),   S(-12, 10), S(-1, -2),
          S(5, -21)},
         {S(-11, 14), S(-15, 23), S(-14, 25), S(-16, 24), S(-4, 13),  S(5, 18),    S(-3, 19),
          S(-4, 14),  S(-21, 17), S(-21, 30), S(-4, 27),  S(6, 20),   S(-5, 16),   S(15, 14),
          S(4, 10),   S(4, 4),    S(-32, 13), S(-11, 17), S(-15, 14), S(-16, 12),  S(9, -1),
          S(11, 2),   S(35, -1),  S(-4, -4),  S(-35, 13), S(-25, 14), S(-23, 15),  S(-19, 11),
          S(-17, -1), S(-11, 2),  S(-15, 5),  S(-21, -3), S(-41, 5),  S(-42, 13),  S(-32, 7),
          S(-29, 8),  S(-24, 0),  S(-36, 10), S(-20, 0),  S(-31, -7), S(-41, 0),   S(-40, 5),
          S(-33, 0),  S(-33, 4),  S(-21, -4), S(-24, -1), S(-2, -18), S(-15, -22), S(-40, -5),
          S(-36, 2),  S(-24, 0),  S(-25, 1),  S(-18, -7), S(-15, -6), S(-8, -17),  S(-32, -18),
          S(-25, 0),  S(-25, 1),  S(-19, 3),  S(-14, 0),  S(-9, -7),  S(-12, -1),  S(-18, -6),
          S(-18, -15)},
         {S(-29, 44), S(-31, 51), S(-16, 62), S(8, 46),   S(4, 49),   S(15, 44),   S(59, -6),
          S(-2, 37),  S(0, 27),   S(-15, 50), S(-16, 77), S(-26, 92), S(-27, 104), S(11, 61),
          S(14, 48),  S(46, 39),  S(0, 31),   S(-2, 40),  S(-4, 64),  S(0, 68),    S(9, 72),
          S(31, 55),  S(38, 34),  S(30, 35),  S(-11, 43), S(-6, 50),  S(-3, 57),   S(-7, 74),
          S(-4, 77),  S(0, 65),   S(5, 62),   S(7, 48),   S(-6, 37),  S(-9, 57),   S(-5, 55),
          S(-2, 68),  S(0, 63),   S(1, 53),   S(8, 47),   S(8, 45),   S(-4, 24),   S(0, 41),
          S(0, 49),   S(0, 45),   S(3, 52),   S(5, 49),   S(16, 35),  S(10, 29),   S(-1, 22),
          S(2, 24),   S(8, 22),   S(11, 29),  S(9, 34),   S(18, 11),  S(20, -4),   S(22, -18),
          S(-4, 20),  S(-4, 19),  S(0, 26),   S(6, 36),   S(7, 23),   S(-2, 16),   S(5, 0),
          S(5, -5)},
         {S(68, -76),   S(72, -26),  S(78, -15), S(-29, 20), S(-9, 0),    S(25, -1),   S(67, -9),
          S(141, -103), S(-55, 1),   S(19, 23),  S(-23, 34), S(50, 23),   S(-1, 27),   S(3, 38),
          S(27, 28),    S(-10, 5),   S(-77, 6),  S(64, 16),  S(-11, 34),  S(-42, 47),  S(-14, 41),
          S(51, 27),    S(16, 27),   S(-35, 4),  S(-53, -6), S(-4, 14),   S(-53, 36),  S(-95, 48),
          S(-105, 45),  S(-65, 36),  S(-61, 24), S(-117, 7), S(-59, -17), S(-18, 8),   S(-63, 31),
          S(-97, 46),   S(-100, 43), S(-59, 27), S(-55, 14), S(-117, 0),  S(-33, -19), S(5, 5),
          S(-38, 24),   S(-44, 33),  S(-43, 31), S(-43, 23), S(-17, 6),   S(-46, -12), S(34, -32),
          S(18, 1),     S(12, 11),   S(-8, 21),  S(-13, 21), S(-1, 12),   S(15, -3),   S(26, -30),
          S(27, -53),   S(46, -23),  S(40, -9),  S(-19, 9),  S(24, -13),  S(-7, 2),    S(35, -19),
          S(41, -60)}}};

    const std::array<EvalScore, 9> knight_mobility = {S(10, -17), S(20, -17), S(25, -10),
                                                      S(27, -13), S(29, -10), S(29, -5),
                                                      S(28, -5),  S(27, -4),  S(28, -8)};

    const std::array<EvalScore, 14> bishop_mobility = {S(-9, -56), S(-1, -46), S(2, -38), S(4, -30),
                                                       S(10, -19), S(15, -9),  S(19, -7), S(22, -2),
                                                       S(23, 2),   S(24, 0),   S(26, -1), S(28, -1),
                                                       S(25, 1),   S(47, -10)};

    const std::array<EvalScore, 15> rook_mobility = {S(-9, -32), S(-2, -16), S(1, -13), S(4, -9),
                                                     S(2, -3),   S(6, -2),   S(6, 0),   S(8, 2),
                                                     S(7, 8),    S(9, 11),   S(11, 13), S(12, 15),
                                                     S(12, 18),  S(14, 20),  S(12, 16)};

    const std::array<EvalScore, 28> queen_mobility = {
        S(-62, -14), S(-59, -108), S(-67, -45), S(-64, -35), S(-64, -28), S(-62, -20), S(-59, -14),
        S(-61, 3),   S(-60, 9),    S(-58, 12),  S(-58, 20),  S(-58, 25),  S(-58, 30),  S(-58, 35),
        S(-57, 39),  S(-56, 42),   S(-55, 45),  S(-57, 53),  S(-54, 55),  S(-53, 54),  S(-44, 51),
        S(-41, 49),  S(-41, 50),   S(-39, 49),  S(-21, 30),  S(20, 15),   S(40, 8),    S(125, -28)};

    const EvalScore stacked_pawn_penalty = S(0, 24);

    const EvalScore bishop_pair_bonus = S(18, 54);

    const std::array<EvalScore, 8> passed_pawn_bonus = {S(10, 20),  S(7, 4),   S(6, 130),
                                                        S(10, 61),  S(-9, 24), S(-8, -9),
                                                        S(-3, -19), S(10, 20)};

    const std::array<EvalScore, 8> rook_open_file_bonus = {S(25, 7),  S(23, 3), S(20, 9), S(23, 7),
                                                           S(23, 12), S(35, 2), S(45, 2), S(59, 2)};

    const std::array<EvalScore, 8> rook_semi_open_file_bonus = {S(0, 36), S(7, 11), S(5, 13),
                                                                S(12, 5), S(7, 7),  S(14, 2),
                                                                S(22, 4), S(2, 26)};

    const std::array<EvalScore, 8> isolated_pawn_penalty = {S(2, -9),  S(2, 9), S(7, 3), S(6, 11),
                                                            S(10, 10), S(2, 3), S(1, 9), S(0, -7)};

    const std::array<EvalScore, 64> knight_outpost_bonus = {
        S(9, 0),    S(7, -1),  S(0, -1),  S(0, -2),   S(-4, -1),  S(3, 0),   S(4, -2),
        S(1, 2),    S(0, 0),   S(0, -1),  S(0, -2),   S(-1, -2),  S(-1, 0),  S(-3, 0),
        S(1, -2),   S(-1, 0),  S(-15, 0), S(17, 14),  S(69, 3),   S(33, 12), S(81, -1),
        S(63, 13),  S(31, 12), S(15, -5), S(17, -16), S(22, 2),   S(25, 9),  S(33, 0),
        S(37, 4),   S(45, -1), S(56, -4), S(30, 3),   S(19, 8),   S(19, 8),  S(25, -1),
        S(23, 0),   S(35, -2), S(37, 0),  S(45, -2),  S(26, 8),   S(0, 0),   S(-1, 1),
        S(5, 4),    S(4, -7),  S(7, -8),  S(5, 3),    S(13, -6),  S(7, -3),  S(-1, -6),
        S(-11, 6),  S(-3, 8),  S(-8, -4), S(-9, -5),  S(-9, -3),  S(-17, 4), S(0, -12),
        S(-6, 1),   S(2, -10), S(-36, 9), S(-21, 8),  S(-19, -4), S(-21, 4), S(-22, 12),
        S(-16, -24)};

    const std::array<EvalScore, 8> supported_pawn_bonus = {S(0, 0),   S(124, 137), S(17, 56),
                                                           S(11, 15), S(12, 4),    S(18, 8),
                                                           S(52, 73), S(0, 0)};

    const std::array<EvalScore, 8> pawn_duo_bonus = {S(0, 0),  S(0, 169), S(56, 78), S(20, 25),
                                                     S(10, 7), S(8, 1),   S(4, 0),   S(0, 0)};

    const std::array<EvalScore, 8> king_open_file_penalty = {S(84, 2),  S(94, 0),  S(58, 2),
                                                             S(53, 0),  S(44, -3), S(75, -8),
                                                             S(70, -6), S(65, -13)};

    const std::array<EvalScore, 8> king_semi_open_file_penalty = {S(31, -50), S(47, -24),
                                                                  S(31, -19), S(16, -8),
                                                                  S(15, -6),  S(32, -15),
                                                                  S(42, -27), S(43, -39)};

    const std::array<EvalScore, 24> pawn_shelter_table = {
        S(-48, 40), S(21, 12), S(-11, -1), S(-2, -5), S(5, -5), S(8, -9), S(9, -9), S(-7, -5),
        S(0, 0),    S(-4, 22), S(-18, -6), S(-1, -2), S(3, -6), S(3, -9), S(2, -8), S(-12, 0),
        S(-15, 30), S(38, -1), S(-21, 1),  S(-8, -2), S(0, -1), S(4, -9), S(3, -9), S(0, -4)};

    const std::array<EvalScore, 24> pawn_storm_table = {
        S(9, 0),  S(13, -7), S(9, 0),  S(2, 2),   S(-6, 9),  S(12, 17),  S(-5, 22),  S(-20, 3),
        S(0, 0),  S(9, -1),  S(2, 3),  S(-7, 0),  S(-14, 2), S(-24, 15), S(-28, 12), S(-4, -1),
        S(22, 6), S(11, -3), S(-9, 1), S(-5, -4), S(-5, -7), S(-6, -6),  S(-18, 8),  S(-17, 0)};

    const std::array<EvalScore, 8> our_king_pp_proximity = {S(0, 0),    S(-5, 26),  S(-9, 12),
                                                            S(-9, -5),  S(-5, -17), S(0, -21),
                                                            S(13, -23), S(7, -24)};

    const std::array<EvalScore, 8> their_king_pp_proximity = {S(0, 0),  S(-66, -20), S(5, -23),
                                                              S(6, 0),  S(10, 10),   S(7, 20),
                                                              S(1, 26), S(-12, 23)};

    const std::array<EvalScore, 8> knight_king_proximity_bonus = {S(0, 0),   S(4, 4), S(2, -3),
                                                                  S(18, -9), S(0, 0), S(0, 0),
                                                                  S(0, 0),   S(0, 0)};

    const std::array<EvalScore, 8> bishop_king_proximity_bonus = {S(0, 0),    S(3, 2),    S(10, -1),
                                                                  S(29, -10), S(10, -17), S(0, 0),
                                                                  S(0, 0),    S(0, 0)};

    const std::array<EvalScore, 8> rook_king_proximity_bonus = {S(0, 0),   S(8, -9),   S(15, -12),
                                                                S(22, -8), S(42, -16), S(36, -15),
                                                                S(0, 0),   S(0, 0)};

    const std::array<EvalScore, 8> queen_king_proximity_bonus = {S(0, 0),   S(1, 7),   S(2, 15),
                                                                 S(12, 25), S(21, 37), S(42, 38),
                                                                 S(91, 29), S(57, 36)};

}