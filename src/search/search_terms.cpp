#include "search_terms.h"

namespace elixir::search {
    // Internal Iterative Reductions
    int IIR_DEPTH = 4;

    // Razoring
    int RAZOR_MARGIN = 311;
    int RAZOR_DEPTH  = 7;

    // Reverse Futility Pruning
    int RFP_MARGIN          = 65;
    int RFP_DEPTH           = 6;
    int RFP_BASE            = 116;
    int RFP_BASE_MULTIPLIER = 39;

    // Null Move Pruning
    int NMP_BASE_REDUCTION = 5;
    int NMP_DEPTH          = 1;
    int NMP_DIVISOR        = 9;
    int NMP_EVAL_BASE      = 202;
    int NMP_EVAL_MAX       = 61;
    int NMP_PHASE_BASE     = 80;
    int NMP_PHASE_MAX      = 24;

    // Singular Extensions
    int SE_BETA_MULTIPLIER = 178;
    int DOUBLE_EXT_MARGIN  = 307;

    // Late Move Pruning
    int LMP_BASE       = 2;
    int LMP_MULTIPLIER = 1;

    // Futility Pruning
    int FP_BASE       = 146;
    int FP_MULTIPLIER = 129;
    int FP_DEPTH      = 5;

    // Late Move Reduction
    int LMR_DEPTH        = 4;
    double LMR_OFFSET    = 0.6682821351207684;
    double LMR_DIVISOR   = 2.362861677508389;
    int LMR_HISTORY_BASE = 7737;

    // Static Exchange Evaluation Pruning
    int SEE_QUIET        = 64;
    int SEE_CAPTURE      = 33;
    int SEE_DEPTH        = 10;
    int QS_SEE_THRESHOLD = 6;

    // Static Exchange Evaluation
    int SEE_PAWN   = 72;
    int SEE_KNIGHT = 294;
    int SEE_BISHOP = 290;
    int SEE_ROOK   = 508;
    int SEE_QUEEN  = 879;

    // Aspiration Windows
    int INITIAL_ASP_DELTA = 15;
    int MIN_ASP_DEPTH     = 4;
    double ASP_MULTIPLIER = 1.002417671948309;

    // SEE pieve values
    int see_pieces[7] = {SEE_PAWN, SEE_KNIGHT, SEE_BISHOP, SEE_ROOK, SEE_QUEEN, 0, 0};
}