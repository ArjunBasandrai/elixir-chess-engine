#include <fstream>
#include <iostream>
#include <vector>
#include <cassert>

#include "texel.h"

#include "../board/board.h"
#include "../defs.h"
#include "../evaluate.h"
#include "../types.h"
#include "../utils/str_utils.h"
#include "../utils/eval_terms.h"

namespace elixir::texel {
    void Tune::get_intial_parameters() {
        add_parameter_array<6>(eval::material_score);
    }

    void Tune::create_entry(Board &board, const std::string line) {
        auto tokens           = str_utils::split(line, '[');
        const auto fen        = tokens[0];
        const auto wdl_result = str_utils::remove_whitespaces_and_brackets(tokens[1]);
        Result result         = static_cast<Result>(std::stoi(wdl_result));
        trace            = Trace();
        board.from_fen(fen);
        const auto phase = board.get_phase();
        eval::evaluate(board);

        TunerPosition position;
        position.phase    = phase;
        position.outcome  = result;
        position.coefficients.reserve(num_params);
        get_all_coefficients(position);
        positions.push_back(position);

        fens.push_back(fen);
    }

    void Tune::read_epd(std::string filename) {
        std::ifstream file(filename);
        if (! file.is_open()) {
            std::cerr << "Error: could not open file " << filename << std::endl;
            return;
        }

        Board board;
        std::string line;
        while (std::getline(file, line)) {
            create_entry(board, line);
        }

        file.close();
    }

    void Tune::load_data(std::vector<std::string> files) {
        std::cout << "Loading data..." << std::endl;
        for (const std::string &file : files) {
            read_epd(file);
        }
        assert(fens.size() == positions.size());
        std::cout << "Loaded " << fens.size() << " positions" << std::endl;
    }
}