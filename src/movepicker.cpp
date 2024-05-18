#include <iostream>

#include "movepicker.h"
#include "move.h"
#include "movegen.h"
#include "evaluate.h"
#include "search.h"

namespace elixir { 
    void MovePicker::setup_mp(const Board& board, move::Move tt_move, search::SearchStack *ss, bool skip_quiets) {
        stage = STAGE::HASH_MOVE;
        no_quiets = skip_quiets;
        noisy_size = 0;
        quiet_size = 0;
        hash_move = tt_move;
        killers[0] = ss->killers[0]; killers[1] = ss->killers[1];
    }

    move::Move MovePicker::next(Board& board) {
        move::Move best_move;

        switch (stage) {
            case STAGE::HASH_MOVE:
                stage = STAGE::GEN_NOISY;
                if (board.is_psuedo_legal(hash_move)) {
                    return hash_move;
                }
            case STAGE::GEN_NOISY:
                movegen::generate_noisy_moves(board, noisy_moves);
                score_noisy(board);
                stage = STAGE::NOISY;
            case STAGE::NOISY:
                if (noisy_size > 0) {
                    int best_idx = 0;
                    for (int i = 1; i < noisy_size; i++) {
                        if (noisy_scores[i] > noisy_scores[best_idx]) {
                            best_idx = i;
                        }
                    }
                    best_move = noisy_moves[best_idx];

                    noisy_size--;
                    noisy_moves[best_idx] = noisy_moves[noisy_size];
                    noisy_scores[best_idx] = noisy_scores[noisy_size];

                    if (best_move == hash_move) {
                        return next(board);
                    }

                    if (best_move == killers[0]) killers[0] = move::NO_MOVE;
                    if (best_move == killers[1]) killers[1] = move::NO_MOVE;

                    return best_move;
                }

                if (no_quiets) {
                    stage = STAGE::END;
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
                movegen::generate_quiet_moves(board, quiet_moves);
                score_quiets(board);
                stage = STAGE::QUIET;
            case STAGE::QUIET:
                if (quiet_size > 0) {
                    int best_idx = 0;
                    for (int i = 1; i < quiet_size; i++) {
                        if (quiet_scores[i] > quiet_scores[best_idx]) {
                            best_idx = i;
                        }
                    }
                    best_move = quiet_moves[best_idx];

                    quiet_size--;
                    quiet_moves[best_idx] = quiet_moves[quiet_size];
                    quiet_scores[best_idx] = quiet_scores[quiet_size];

                    if (best_move == hash_move 
                        || best_move == killers[0]
                        || best_move == killers[1]) {
                        return next(board);
                    }

                    return best_move;
                }
                stage = STAGE::END;

            case STAGE::END:
                return move::NO_MOVE;
            
            default:
                assert(0);
                return move::NO_MOVE;

        }
    }

    void MovePicker::score_noisy(const Board& board) {
        int value;
        move::Move move;

        noisy_scores.resize(noisy_moves.size());

        noisy_size = noisy_moves.size();

        for (int i = 0; i < noisy_size; i++) {
            move = noisy_moves[i];
            auto from = move.get_from();
            auto to = move.get_to();
            auto from_piece = static_cast<int>(board.piece_to_piecetype(move.get_piece()));
            auto to_piece = static_cast<int>(board.piece_to_piecetype(board.piece_on(to)));
            auto to_val = eval::piece_values[to_piece];

            value = 5 * to_val - from_piece;

            if (move.is_promotion() && move.get_promotion() == move::Promotion::QUEEN) {
                value += 5 * eval::piece_values[static_cast<int>(PieceType::QUEEN)];
            } else if (move.is_promotion() && move.get_promotion() == move::Promotion::ROOK) {
                value += 5 * eval::piece_values[static_cast<int>(PieceType::ROOK)];
            } else if (move.is_promotion() && move.get_promotion() == move::Promotion::BISHOP) {
                value += 5 * eval::piece_values[static_cast<int>(PieceType::BISHOP)];
            } else if (move.is_promotion() && move.get_promotion() == move::Promotion::KNIGHT) {
                value += 5 * eval::piece_values[static_cast<int>(PieceType::KNIGHT)];
            } else if (move.is_en_passant()) {
                value = 2 * eval::piece_values[static_cast<int>(PieceType::PAWN)];
            }

            noisy_scores[i] = value;
        }
    }

    void MovePicker::score_quiets(const Board& board) {
        int value;
        move::Move move;

        quiet_scores.resize(quiet_moves.size());

        quiet_size = quiet_moves.size();

        for (int i = 0; i < quiet_size; i++) {
            move = quiet_moves[i];
            auto from = static_cast<int>(move.get_from());
            auto to = static_cast<int>(move.get_to());
            auto piece = static_cast<int>(board.piece_to_piecetype(move.get_piece()));

            value = board.history[from][to];
            value += std::abs(O(eval::psqt[piece][to]));
            value -= std::abs(O(eval::psqt[piece][from]));

            quiet_scores[i] = value;
        }
    }
}