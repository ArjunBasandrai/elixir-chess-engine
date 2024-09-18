#pragma once

#include "defs.h"

namespace elixir::search {
    // Internal Iterative Reductions
    extern int IIR_DEPTH;

    // Razoring
    extern int RAZOR_MARGIN;
    extern int RAZOR_DEPTH;

    // Reverse Futility Pruning
    extern int RFP_MARGIN;
    extern int RFP_DEPTH;
    extern int RFP_BASE;
    extern int RFP_BASE_MULTIPLIER;

    // Null Move Pruning
    extern int NMP_BASE_REDUCTION;
    extern int NMP_DEPTH;
    extern int NMP_DIVISOR;
    extern int NMP_EVAL_BASE;
    extern int NMP_EVAL_MAX;
    extern int NMP_PHASE_BASE;
    extern int NMP_PHASE_MAX;

    // Singular Extensions
    extern int SE_BETA_MULTIPLIER;
    extern int DOUBLE_EXT_MARGIN;

    // Late Move Pruning
    extern int LMP_BASE;
    extern int LMP_MULTIPLIER;

    // Futility Pruning
    extern int FP_BASE;
    extern int FP_MULTIPLIER;
    extern int FP_DEPTH;

    // Late Move Reduction
    extern int LMR_DEPTH;
    extern double LMR_OFFSET;
    extern double LMR_DIVISOR;
    extern int LMR_HISTORY_BASE;

    // Static Exchange Evaluation Pruning
    extern int SEE_QUIET;
    extern int SEE_CAPTURE;
    extern int SEE_DEPTH;
    extern int QS_SEE_THRESHOLD;

    // Static Exchange Evaluation
    extern int SEE_PAWN;
    extern int SEE_KNIGHT;
    extern int SEE_BISHOP;
    extern int SEE_ROOK;
    extern int SEE_QUEEN;

    // Aspiration Windows
    extern int INITIAL_ASP_DELTA;
    extern int MIN_ASP_DEPTH;
    extern double ASP_MULTIPLIER;

    // SEE piece values
    extern int see_pieces[7];

    // Late Move Reductions Table
    extern int lmr[MAX_DEPTH][64];
}