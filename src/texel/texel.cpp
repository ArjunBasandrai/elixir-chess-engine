#include <cassert>
#include <chrono>
#include <fstream>
#include <iostream>
#include <vector>

#include "texel.h"

#include "../board/board.h"
#include "../defs.h"
#include "../evaluate.h"
#include "../types.h"
#include "../utils/eval_terms.h"
#include "../utils/str_utils.h"

namespace elixir::texel {
    void Tune::get_intial_parameters() {
        start_time = std::chrono::high_resolution_clock::now();
        std::cout << "[" << time_spent() << "s]" << " Getting initial parameters...\n" << std::endl;
        add_parameter_array<6>(eval::material_score);
        add_parameter_matrix<6, 64>(eval::psqt);
        add_parameter_array<9>(eval::knight_mobility);
        add_parameter_array<14>(eval::bishop_mobility);
        add_parameter_array<15>(eval::rook_mobility);
        add_parameter_array<28>(eval::queen_mobility);
    }

    void Tune::create_entry(Board &board, const std::string line) {
        auto tokens           = str_utils::split(line, '[');
        const auto fen        = tokens[0];
        const auto wdl_result = str_utils::remove_whitespaces_and_brackets(tokens[1]);
        Result result         = static_cast<Result>((std::stod(wdl_result) * 2) - 1);
        trace                 = Trace();

        board.from_fen(fen);
        const auto phase = board.get_phase();

        TunerPosition position;
        position.eval = eval::evaluate(board) * color_offset[static_cast<int>(board.get_side_to_move())];
        position.phase   = phase;
        position.outcome = result;
        position.coefficients.reserve(num_params);
        get_all_coefficients(position);
        positions.push_back(position);

        fens.push_back(fen);
    }

    void Tune::read_epd(std::string filename) {
        std::cout << "Reading " << filename << "...\n" << std::endl;
        std::ifstream file(filename);
        if (! file.is_open()) {
            std::cerr << "ERROR: could not open file " << filename << std::endl;
            return;
        }

        Board board;
        std::string line;
        int count = 0;
        while (std::getline(file, line)) {
            create_entry(board, line);
            if (++count % 10000 == 0) {
                std::cout << "[" << time_spent() << "s]"
                          << " Read " << count << " positions" << std::endl;
            }
        }
        std::cout << std::endl;
        file.close();
    }

    void Tune::load_data(std::vector<std::string> files) {
        std::cout << "Loading data...\n " << std::endl;
        for (const std::string &file : files) {
            read_epd(file);
        }
        assert(fens.size() == positions.size());
        std::cout << "Loaded " << fens.size() << " positions" << std::endl;
    }

    int Tune::get_eval(const TunerPosition &position) const {
        Score mg = 0, eg = 0;
        #pragma omp parallel shared(mg, eg) num_threads(2)
        {
            #pragma omp for schedule(static) reduction(+ : mg, eg)
            for (const Coefficient &coeff : position.coefficients) {
                mg += parameters[coeff.index][0] * coeff.value;
                eg += parameters[coeff.index][1] * coeff.value;
            }
        }
        return (mg * position.phase + eg * (24 - position.phase)) / 24;
    }

    double Tune::get_error(const double K) const {
        double error = 0.0;

#pragma omp parallel shared(error) num_threads(6)
        {
#pragma omp for schedule(static) reduction(+ : error)
            for (const TunerPosition &position : positions) {
                error +=
                    std::pow((double)((position.outcome + 1) / 2) - sigmoid(position.eval, K), 2);
            }
        }

        return error / positions.size();
    }

    double Tune::get_tuned_error(const double K) const {
        double error = 0.0;

#pragma omp parallel shared(error) num_threads(6)
        {
#pragma omp for schedule(static) reduction(+ : error)
            for (const auto &position : positions) {
                error +=
                    std::pow((double)((position.outcome + 1) / 2) - sigmoid(get_eval(position), K), 2);
            }
        }

        return error / positions.size();
    }

    void Tune::set_optimal_k() {
        const double rate = 10, delta = 1e-5, deviation_goal = 1e-7;
        double deviation   = 1;
        hyper_parameters.K = 2.5;

        while (fabs(deviation) > deviation_goal) {
            double up   = get_error(hyper_parameters.K + delta);
            double down = get_error(hyper_parameters.K - delta);
            deviation   = (up - down) / (2 * delta);
            hyper_parameters.K -= rate * deviation;
            std::cout << "[" << time_spent() << "s]"
                      << " K: " << hyper_parameters.K << " up: " << up << " down: " << down
                      << " deviation: " << deviation << std::endl;
        }

        std::cout << "[" << time_spent() << "s]"
                  << " Optimal K: " << hyper_parameters.K << std::endl;
    }

    void Tune::get_gradients() {
        gradients.reserve(num_params);


        for (int i = 0; i < num_params; i++) {
            gradients.push_back({0, 0});
        }

        for (const auto &position : positions) {
            int E = get_eval(position);
            double S      = sigmoid(E, hyper_parameters.K);
            double result = (position.outcome + 1) / 2.0;
            double D      = (result - S) * S * (1 - S);

            double mg_base = D * (position.phase / 24.0);
            double eg_base = D - mg_base;

            for (const Coefficient &coeff : position.coefficients) {
                const auto idx = coeff.index;
                gradients[idx][0] += mg_base * coeff.value;
                gradients[idx][1] += eg_base * coeff.value;
            }
        }
    }

    void Tune::tune() {
        std::vector<pair_t> velocity, momentum;
        velocity.resize(num_params);
        momentum.resize(num_params);

        const std::size_t num_entries = positions.size();

        set_optimal_k();

        for (int epoch = 0; epoch < hyper_parameters.max_epochs; epoch++) {
            get_gradients();

            for (int i = 0; i < num_params; i++) {
                pair_t grads;
                grads[0] = (-hyper_parameters.K / 400.0) * gradients[i][0] / num_entries;
                grads[1] = (-hyper_parameters.K / 400.0) * gradients[i][1] / num_entries;

                momentum[i][0] = hyper_parameters.momentum_coeff * momentum[i][0] +
                                 (1 - hyper_parameters.momentum_coeff) * grads[0];
                momentum[i][1] = hyper_parameters.momentum_coeff * momentum[i][1] +
                                 (1 - hyper_parameters.momentum_coeff) * grads[1];

                velocity[i][0] = hyper_parameters.velocity_coeff * velocity[i][0] +
                                 (1 - hyper_parameters.velocity_coeff) * std::pow(grads[0], 2);
                velocity[i][1] = hyper_parameters.velocity_coeff * velocity[i][1] +
                                 (1 - hyper_parameters.velocity_coeff) * std::pow(grads[1], 2);

                pair_t update;
                update[0] = hyper_parameters.learning_rate * momentum[i][0] /
                            (hyper_parameters.epsilon + std::sqrt(velocity[i][0]));
                update[1] = hyper_parameters.learning_rate * momentum[i][1] /
                            (hyper_parameters.epsilon + std::sqrt(velocity[i][1]));

                parameters[i][0] -= update[0];
                parameters[i][1] -= update[1];
            }

            const double error = get_tuned_error(hyper_parameters.K);
            std::cout << "[" << time_spent() << "s]"
                      << " Epoch: " << epoch + 1 << " Error: " << error
                      << " Rate: " << hyper_parameters.learning_rate << std::endl;
            if (error < best_params.error) {
                best_params.error  = error;
                best_params.epoch  = epoch;
                best_params.params = parameters;
            } else {
                if (epoch - best_params.epoch >= hyper_parameters.early_stopping) {
                    std::cout << "[" << time_spent() << "s]"
                              << " Early stopping at epoch " << epoch << std::endl;
                    std::cout << "[" << time_spent() << "s]"
                              << " Restoring best parameters from epoch " << best_params.epoch + 1
                              << "..." << std::endl;
                    parameters = best_params.params;
                    break;
                }
            }

            if ((epoch + 1) % hyper_parameters.lr_decay_interval == 0 && epoch) {
                hyper_parameters.learning_rate *= hyper_parameters.lr_decay;
            }
        }
    }
}