#include <cassert>
#include <fstream>
#include <iostream>
#include <vector>
#include <chrono>

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
        add_parameter_array<6>(eval::material_score);
    }

    void Tune::create_entry(Board &board, const std::string line) {
        auto tokens           = str_utils::split(line, '[');
        const auto fen        = tokens[0];
        const auto wdl_result = str_utils::remove_whitespaces_and_brackets(tokens[1]);
        Result result         = static_cast<Result>((std::stod(wdl_result) * 2) - 1);
        trace                 = Trace();

        board.from_fen(fen);
        const auto phase = board.get_phase();

        eval::evaluate(board);

        TunerPosition position;
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
                std::cout << "[" << time_spent() << "s]" << " Read " << count << " positions" << std::endl;
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

    void Tune::get_eval() {
        std::cout << "Getting eval..." << std::endl;
        for (TunerPosition &position : positions) {
            EvalScore eval = 0;
            for (const Coefficient &coeff : position.coefficients) {
                const auto param        = parameters[coeff.index];
                const auto scaled_param = S(param[0], param[1]) * coeff.value;
                eval += scaled_param;
            }
            const auto tapered_eval =
                (O(eval) * position.phase + E(eval) * (24 - position.phase)) / 24;
            position.eval = tapered_eval;
        }
    }

    double Tune::get_error(const double K) const {
        double error = 0.0;
        
        #pragma omp parallel shared(error) num_threads(6)
        {
            #pragma omp for schedule(static) reduction(+ : error)
            for (const TunerPosition &position : positions) {
                error += std::pow((double)((position.outcome + 1) / 2) - sigmoid(position.eval, K), 2);
            }
        }

        return error / positions.size();
    }

    void Tune::set_optimal_k() {
        const double rate = 10, delta = 1e-5, deviation_goal = 1e-6;
        double deviation   = 1;
        hyper_parameters.K = 2.5;

        while (fabs(deviation) > deviation_goal) {
            double up   = get_error(hyper_parameters.K + delta);
            double down = get_error(hyper_parameters.K - delta);
            deviation   = (up - down) / (2 * delta);
            hyper_parameters.K -= rate * deviation;
            std::cout << "[" << time_spent() << "s]" << " K: " << hyper_parameters.K << " up: " << up << " down: " << down
                      << " deviation: " << deviation << std::endl;
        }

        std::cout << "[" << time_spent() << "s]" << " Optimal K: " << hyper_parameters.K << std::endl;
    }
}