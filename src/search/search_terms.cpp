#include "search_terms.h"

namespace elixir::search {
    // Internal Iterative Reductions
    int IIR_DEPTH = 4;

    // Razoring
    int RAZOR_MARGIN = 298;
    int RAZOR_DEPTH  = 7;

    // Reverse Futility Pruning
    int RFP_MARGIN = 72;
    int RFP_DEPTH  = 6;

    // Null Move Pruning
    int NMP_BASE_REDUCTION = 5;
    int NMP_DEPTH          = 1;
    int NMP_DIVISOR        = 9;

    // Late Move Pruning
    int LMP_BASE       = 2;
    int LMP_MULTIPLIER = 1;

    // Futility Pruning
    int FP_BASE       = 146;
    int FP_MULTIPLIER = 118;
    int FP_DEPTH      = 5;

    // Late Move Reduction
    int LMR_DEPTH      = 4;
    double LMR_OFFSET  = 0.5137;
    double LMR_DIVISOR = 1.711;

    // Static Exchange Evaluation Pruning
    int SEE_QUIET        = 67;
    int SEE_CAPTURE      = 32;
    int SEE_DEPTH        = 10;
    int QS_SEE_THRESHOLD = 9;

    // Static Exchange Evaluation
    int SEE_PAWN   = 77;
    int SEE_KNIGHT = 299;
    int SEE_BISHOP = 303;
    int SEE_ROOK   = 504;
    int SEE_QUEEN  = 903;

    // Aspiration Windows
    int INITIAL_ASP_DELTA = 12;
    int MIN_ASP_DEPTH     = 4;
    double ASP_MULTIPLIER = 1.3111;

    // SEE pieve values
    int see_pieces[7] = {SEE_PAWN, SEE_KNIGHT, SEE_BISHOP, SEE_ROOK, SEE_QUEEN, 0, 0};
}