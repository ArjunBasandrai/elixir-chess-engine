#include <climits>
#include <iostream>

#include "evaluate.h"
#include "move.h"
#include "movegen.h"
#include "movepicker.h"
#include "search.h"
#include "ss.h"

namespace elixir {

    int MP_SEE = 109;

    void MovePicker::setup_mp(const Board& board, move::Move tt_move, search::SearchStack *ss, bool skip_quiets) {
        stage = STAGE::HASH_MOVE;
        no_quiets = skip_quiets;
        noisy_size = 0;
        quiet_size = 0;
        hash_move = tt_move;
        killers[0] = (ss->killers[0] != tt_move) ? ss->killers[0] : move::NO_MOVE; 
        killers[1] = (ss->killers[1] != tt_move) ? ss->killers[1] : move::NO_MOVE;
    }

    move::Move MovePicker::next(Board& board, search::SearchStack *ss) {
        move::Move best_move = move::NO_MOVE;

        switch (stage) {
            case STAGE::HASH_MOVE:
                stage = STAGE::GEN_NOISY;
                if (board.is_psuedo_legal(hash_move)) {
                    return hash_move;
                }
            case STAGE::GEN_NOISY:
                moves = movegen::generate_moves<true>(board);
                score_moves(board, hash_move, ss);
                noisy_size = moves.size();
                stage = STAGE::NOISY;
            case STAGE::NOISY:
                if (noisy_size > 0) {
                    int best_idx = 0;
                    for (int i = 1; i < noisy_size; i++) {
                        if (scores[i] > scores[best_idx]) {
                            best_idx = i;
                        }
                    }
                    best_move = moves[best_idx];

                    noisy_size--;
                    std::swap(moves[best_idx], moves[noisy_size]);
                    std::swap(scores[best_idx], scores[noisy_size]);

                    if (!search::SEE(board, best_move, -MP_SEE)) {
                        return next(board, ss);
                    }

                    if (best_move == hash_move) {
                        return next(board, ss);
                    }

                    if (best_move == killers[0]) { killers[0] = move::NO_MOVE; }
                    if (best_move == killers[1]) { killers[1] = move::NO_MOVE; }

                    return best_move;
                }

                if (no_quiets) {
                    stage = STAGE::GEN_BAD_CAPTURES;
                    return move::NO_MOVE;
                } else {
                    stage = STAGE::KILLER_1;
                }
            case STAGE::KILLER_1:
                stage = STAGE::KILLER_2;
                if (killers[0] != move::NO_MOVE && board.is_psuedo_legal(killers[0])) {
                    return killers[0];
                }
            case STAGE::KILLER_2:
                stage = STAGE::GEN_QUIET;
                if (killers[1] != move::NO_MOVE && board.is_psuedo_legal(killers[1])) {
                    return killers[1];
                }
            case STAGE::GEN_QUIET:
                moves = movegen::generate_moves<false>(board);
                score_moves(board, hash_move, ss);
                quiet_size = moves.size();
                stage = STAGE::QUIET;
            case STAGE::QUIET:
                if (quiet_size > 0) {
                    int best_idx = 0;
                    for (int i = 1; i < quiet_size; i++) {
                        if (scores[i] > scores[best_idx]) {
                            best_idx = i;
                        }
                    }
                    best_move = moves[best_idx];

                    quiet_size--;
                    std::swap(moves[best_idx], moves[quiet_size]);
                    std::swap(scores[best_idx], scores[quiet_size]);

                    if (best_move == hash_move || best_move == killers[0] || best_move == killers[1]) {
                        return next(board, ss);
                    }

                    return best_move;
                }
                stage = STAGE::GEN_BAD_CAPTURES;
            case STAGE::GEN_BAD_CAPTURES:
                moves = movegen::generate_moves<true>(board);
                score_moves(board, hash_move, ss);
                noisy_size = moves.size();
                stage = STAGE::BAD_CAPTURES;
            case STAGE::BAD_CAPTURES:
                if (noisy_size > 0) {
                    int best_idx = 0;
                    for (int i = 1; i < noisy_size; i++) {
                        if (scores[i] > scores[best_idx]) {
                            best_idx = i;
                        }
                    }
                    best_move = moves[best_idx];

                    noisy_size--;
                    std::swap(moves[best_idx], moves[noisy_size]);
                    std::swap(scores[best_idx], scores[noisy_size]);

                    if (search::SEE(board, best_move, -MP_SEE)) {
                        return next(board, ss);
                    }

                    if (best_move == hash_move || best_move == killers[0] || best_move == killers[1]) {
                        return next(board, ss);
                    }

                    return best_move;
                }
            case STAGE::END:
                return move::NO_MOVE;
            default:
                assert(0);
                return move::NO_MOVE;
        }
    }

    void MovePicker::score_moves(const Board &board, const move::Move &tt_move,
                                 const search::SearchStack *ss) {
        scores.resize(moves.size());
        scores.fill(0);

        int value;
        Square from, to;

        for (int i = 0; i < moves.size(); i++) {

            move::Move move = moves[i];

            value = 0;

            from = move.get_from();
            to   = move.get_to();

            // Move Ordering (~450 ELO)
            if (move.is_promotion()) {
                switch (move.get_promotion()) {
                    case move::Promotion::QUEEN:
                        value = 2000000001;
                        break;
                    case move::Promotion::KNIGHT:
                        value = 2000000000;
                        break;
                    case move::Promotion::ROOK:
                        value = -2000000001;
                        break;
                    case move::Promotion::BISHOP:
                        value = -2000000000;
                        break;
                    default:
                        break;
                }
            } else if (move.is_capture() || move.is_en_passant()) {
                auto captured_piece =
                    move.is_en_passant()
                        ? static_cast<int>(PieceType::PAWN)
                        : static_cast<int>(board.piece_to_piecetype(board.piece_on(to)));
                value = eval::piece_values[captured_piece];
                value += search::SEE(board, move, -MP_SEE) ? 1000000000 : -1000000;
            } else if (move == board.history.get_countermove(board.get_side_to_move(),
                                                             (ss - 1)->move.get_from(),
                                                             (ss - 1)->move.get_to())) {
                value = 600000000;
            } else {
                // Butterfly History Move Ordering (~45 ELO)
                value = board.history.get_history(move, ss);
            }

            scores[i] = value;
        }
    }

    void MovePicker::init_mp(const Board &board, move::Move tt_move, search::SearchStack *ss,
                             bool for_qs) {
        if (for_qs)
            moves = movegen::generate_moves<true>(board);
        else
            moves = movegen::generate_moves<false>(board);
        score_moves(board, tt_move, ss);
    }

    move::Move MovePicker::next_move() {
        if (moves.size() <= 0)
            return move::NO_MOVE;

        // (~300 ELO)
        int max = -INF, max_idx = 0;
        for (int i = 0; i < moves.size(); i++) {
            if (scores[i] > max) {
                max     = scores[i];
                max_idx = i;
            }
        }

        auto best_move = moves[max_idx];

        int last_index = moves.size() - 1;
        std::swap(moves[max_idx], moves[last_index]);
        std::swap(scores[max_idx], scores[last_index]);

        moves.pop_back();
        scores.pop_back();

        return best_move;
    }
}