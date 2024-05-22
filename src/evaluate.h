#pragma once

#include "board/board.h"

namespace elixir {
    constexpr inline EvalScore S(Score o_score, Score e_score) {
        return static_cast<EvalScore>((o_score << 16) | (e_score & 0xFFFF));
    }

    constexpr inline Score O(EvalScore score) {
        return static_cast<Score>(score >> 16);
    }

    constexpr inline Score E(EvalScore score) {
        return static_cast<Score>(score & 0xFFFF);
    }

    namespace eval {
        constexpr int piece_values[7] = {100, 300, 350, 500, 900, 20000, 0};
        constexpr EvalScore material_score[7] = {S(103, 95), S(429, 221), S(418, 208), S(557, 344), S(1040, 612), S(0, 0), S(0, 0)};
        constexpr EvalScore psqt[6][64] = {
            {
                S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), 
                S(98, 286), S(78, 287), S(64, 262), S(106, 213), S(142, 195), S(124, 217), S(27, 279), 
                S(-34, 275), S(-18, 190), S(15, 183), S(47, 163), S(36, 137), S(68, 113), S(117, 100), 
                S(32, 148), S(-4, 159), S(-4, 92), S(28, 75), S(20, 57), S(38, 40), S(58, 26), 
                S(34, 39), S(38, 56), S(-10, 61), S(-34, 63), S(19, 50), S(15, 28), S(45, 22), 
                S(43, 21), S(30, 20), S(36, 32), S(-29, 37), S(-24, 51), S(15, 47), S(25, 25), 
                S(16, 41), S(43, 32), S(12, 33), S(84, 28), S(19, 27), S(-28, 67), S(32, 53), 
                S(6, 54), S(0, 56), S(24, 42), S(69, 38), S(109, 28), S(20, 27), S(0, 0), 
                S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0)
            }, 
            {
                S(-184, 103), S(89, 46), S(129, 84), S(92, 97), S(240, 62), S(5, 86), S(-35, 87), S(-23, 0), 
                S(95, 73), S(155, 91), S(300, 75), S(229, 99), S(316, 66), S(303, 67), S(192, 70), 
                S(190, 41), S(189, 75), S(232, 90), S(236, 132), S(296, 124), S(338, 87), S(375, 93), 
                S(296, 76), S(188, 84), S(218, 90), S(223, 130), S(222, 138), S(278, 147), S(233, 144), 
                S(268, 133), S(205, 126), S(237, 84), S(184, 95), S(201, 115), S(224, 142), S(220, 144), 
                S(235, 147), S(238, 124), S(210, 103), S(168, 102), S(172, 81), S(194, 111), S(228, 104), 
                S(219, 137), S(234, 133), S(218, 113), S(232, 93), S(167, 71), S(164, 69), S(155, 84), 
                S(193, 93), S(204, 110), S(205, 106), S(221, 99), S(195, 80), S(186, 69), S(110, 37), 
                S(171, 55), S(125, 92), S(155, 102), S(155, 90), S(176, 100), S(165, 54), S(81, 68)
            }, 
            {
                S(202, 139), S(202, 120), S(231, 124), S(176, 150), S(89, 158), S(199, 143), S(271, 118), S(149, 153), 
                S(226, 125), S(283, 142), S(248, 148), S(255, 136), S(285, 146), S(351, 122), S(269, 146), 
                S(294, 95), S(243, 151), S(274, 151), S(333, 149), S(296, 165), S(354, 139), S(340, 159), 
                S(330, 143), S(318, 128), S(225, 159), S(262, 168), S(296, 159), S(305, 171), S(306, 168), 
                S(271, 155), S(280, 149), S(252, 136), S(247, 138), S(279, 149), S(279, 168), S(311, 159), 
                S(314, 160), S(285, 168), S(277, 138), S(229, 133), S(298, 120), S(292, 148), S(294, 156), 
                S(283, 165), S(293, 168), S(294, 152), S(283, 130), S(289, 119), S(245, 136), S(309, 117), 
                S(277, 133), S(273, 150), S(275, 155), S(308, 136), S(323, 129), S(278, 111), S(276, 103), 
                S(235, 133), S(263, 106), S(244, 144), S(234, 141), S(237, 128), S(266, 111), S(253, 119)
            }, 
            {
                S(392, 322), S(415, 311), S(433, 310), S(439, 302), S(488, 287), S(532, 269), S(463, 282), S(375, 306), 
                S(340, 320), S(322, 339), S(381, 322), S(434, 303), S(381, 306), S(458, 293), S(470, 284), 
                S(425, 284), S(326, 319), S(339, 317), S(342, 316), S(380, 305), S(406, 293), S(426, 289), 
                S(454, 285), S(411, 284), S(281, 322), S(312, 313), S(349, 314), S(367, 306), S(337, 310), 
                S(353, 305), S(388, 289), S(381, 285), S(276, 315), S(277, 319), S(283, 324), S(309, 321), 
                S(318, 313), S(311, 307), S(338, 290), S(317, 284), S(271, 302), S(291, 308), S(312, 305), 
                S(327, 302), S(323, 304), S(311, 296), S(358, 275), S(333, 276), S(260, 311), S(296, 306), 
                S(294, 317), S(305, 314), S(310, 306), S(316, 304), S(342, 293), S(251, 306), S(298, 304), 
                S(304, 317), S(328, 314), S(331, 327), S(337, 305), S(317, 300), S(288, 308), S(315, 258)
            }, 
            {
                S(726, 616), S(785, 612), S(847, 598), S(904, 592), S(935, 566), S(934, 573), S(765, 607), S(733, 663), 
                S(772, 596), S(759, 607), S(760, 644), S(720, 712), S(747, 688), S(953, 636), S(828, 654), 
                S(999, 462), S(776, 563), S(786, 596), S(814, 635), S(846, 613), S(860, 676), S(928, 593), 
                S(977, 545), S(898, 550), S(764, 579), S(786, 619), S(796, 615), S(792, 657), S(798, 685), 
                S(811, 679), S(790, 655), S(829, 604), S(785, 580), S(795, 606), S(793, 612), S(798, 656), 
                S(812, 635), S(808, 619), S(817, 615), S(794, 588), S(787, 543), S(818, 534), S(803, 621), 
                S(811, 590), S(806, 618), S(818, 605), S(821, 609), S(802, 568), S(781, 577), S(816, 578), 
                S(820, 562), S(817, 589), S(825, 573), S(848, 515), S(853, 502), S(825, 526), S(810, 553), 
                S(787, 547), S(802, 549), S(828, 491), S(795, 570), S(776, 544), S(733, 566), S(796, 511)
            }, 
            {
                S(198, -101), S(125, -44), S(159, -47), S(142, -38), S(168, -35), S(146, -18), S(95, -18), S(100, -58), 
                S(85, -36), S(196, -15), S(129, -11), S(94, -8), S(100, -3), S(190, 10), S(31, 31), 
                S(-33, 16), S(84, -29), S(222, -12), S(63, 9), S(105, 2), S(23, 19), S(115, 25), 
                S(134, 31), S(-17, 22), S(23, -38), S(25, -2), S(71, 9), S(0, 26), S(-22, 28), 
                S(-7, 37), S(18, 26), S(-59, 14), S(66, -55), S(46, -12), S(14, 16), S(-93, 42), 
                S(-78, 45), S(-106, 49), S(-51, 24), S(-121, 10), S(18, -46), S(29, -9), S(-102, 31), 
                S(-124, 47), S(-127, 51), S(-90, 42), S(-20, 16), S(-55, 0), S(52, -47), S(-23, -5), 
                S(-53, 19), S(-132, 41), S(-109, 40), S(-61, 30), S(25, 0), S(25, -26), S(-82, -39), 
                S(30, -41), S(-10, -20), S(-115, -2), S(-15, -37), S(-68, -8), S(46, -50), S(37, -83)
            }
        };
        Phase get_game_phase(Board &board, int game_phase_score);
        int evaluate(Board& board);
        Score interpolate_eval(EvalScore score, Board &board);
    }
}