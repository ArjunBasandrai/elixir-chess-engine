#include "search_terms.h"

namespace elixir::search {
    // Internal Iterative Reductions
    int IIR_DEPTH = 4;

    // Razoring
    int RAZOR_MARGIN = 308;
    int RAZOR_DEPTH  = 7;

    // Reverse Futility Pruning
    int RFP_MARGIN          = 61;
    int RFP_DEPTH           = 6;
    int RFP_BASE            = 118;
    int RFP_BASE_MULTIPLIER = 39;

    // Null Move Pruning
    int NMP_BASE_REDUCTION = 5;
    int NMP_DEPTH          = 1;
    int NMP_DIVISOR        = 9;
    int NMP_EVAL_BASE      = 208;
    int NMP_EVAL_MAX       = 60;
    int NMP_PHASE_BASE     = 81;
    int NMP_PHASE_MAX      = 24;

    // Singular Extensions
    int SE_BETA_MULTIPLIER = 183;
    int DOUBLE_EXT_MARGIN  = 309;

    // Late Move Pruning
    int LMP_BASE       = 2;
    int LMP_MULTIPLIER = 1;

    // Futility Pruning
    int FP_BASE       = 139;
    int FP_MULTIPLIER = 117;
    int FP_DEPTH      = 5;

    // Late Move Reduction
    int LMR_DEPTH        = 4;
    double LMR_OFFSET    = 0.5788355002155825;
    double LMR_DIVISOR   = 2.364236840738073;
    int LMR_HISTORY_BASE = 7788;

    // Static Exchange Evaluation Pruning
    int SEE_QUIET        = 68;
    int SEE_CAPTURE      = 31;
    int SEE_DEPTH        = 10;
    int QS_SEE_THRESHOLD = 8;

    // Static Exchange Evaluation
    int SEE_PAWN   = 74;
    int SEE_KNIGHT = 293;
    int SEE_BISHOP = 293;
    int SEE_ROOK   = 504;
    int SEE_QUEEN  = 885;

    // Aspiration Windows
    int INITIAL_ASP_DELTA = 14;
    int MIN_ASP_DEPTH     = 4;
    double ASP_MULTIPLIER = 1.0012189158768376;

    // SEE pieve values
    int see_pieces[7] = {SEE_PAWN, SEE_KNIGHT, SEE_BISHOP, SEE_ROOK, SEE_QUEEN, 0, 0};
}