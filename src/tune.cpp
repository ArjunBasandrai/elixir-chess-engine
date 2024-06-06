#ifdef USE_TUNE
#include <variant>

#include "tune.h"
#include "movepicker.h"
#include "search.h"
#include "evaluate.h"
#endif

namespace elixir {
    int DEFAULT_MOVESTOGO = 19;
    int DEFAULT_MOVE_OVERHEAD = 48;
    int INCREMENT_SCALE = 1;
}

#ifdef USE_TUNE
namespace elixir::tune {
    
    Tuner tuner;

    void Tuner::add_field(TunerField field) {
        fields.push_back(field);
    }

    void Tuner::print_info() {
        for (auto& field : fields) {
            const bool is_int = std::holds_alternative<int>(field.default_value);
            std::cout << "option name " << field.name;
            std::cout << " type " << (is_int ? "spin" : "string");
            std::cout << " default " << (is_int ? std::get<int>(field.default_value) : std::get<double>(field.default_value));
            std::cout << " min " << (is_int ? std::get<int>(field.min_value) : std::get<double>(field.min_value));
            std::cout << " max " << (is_int ? std::get<int>(field.max_value) : std::get<double>(field.max_value));
            std::cout << std::endl;
        }
    }

    void Tuner::print_current() {
        for (auto& field : fields) {
            const bool is_int = std::holds_alternative<int>(field.default_value);
            std::cout << field.name << ": ";
            std::cout << (is_int ? *std::get<int*>(field.value) : *std::get<double*>(field.value));
            std::cout << std::endl;
        }
    }

    void init_tune() {
        using namespace elixir::eval;
        using namespace elixir::search;
        // eval terms
        tuner.add_field({"TEMPO", &TEMPO, 7, 1, 100, 1, 0.002});

        // movepicker terms
        tuner.add_field({"MP_PAWN", &MP_PAWN, 86, 50, 300, 10, 0.002});
        tuner.add_field({"MP_KNIGHT", &MP_KNIGHT, 304, 150, 500, 10, 0.002});
        tuner.add_field({"MP_BISHOP", &MP_BISHOP, 360, 200, 600, 10, 0.002});
        tuner.add_field({"MP_ROOK", &MP_ROOK, 466, 300, 800, 25, 0.002});
        tuner.add_field({"MP_QUEEN", &MP_QUEEN, 905, 500, 1300, 40, 0.002});
        tuner.add_field({"MP_KING", &MP_KING, 20903, 10000, 30000, 500, 0.002});
        tuner.add_field({"MP_SEE", &MP_SEE, 109, 20, 500, 5, 0.002});

        // history terms
        tuner.add_field({"HISTORY_GRAVITY", &HISTORY_GRAVITY, 8289, 1024, 16384, 32, 0.002});

        // time management terms
        tuner.add_field({"DEFAULT_MOVESTOGO", &DEFAULT_MOVESTOGO, 19, 5, 50, 1, 0.002});
        tuner.add_field({"DEFAULT_MOVE_OVERHEAD", &DEFAULT_MOVE_OVERHEAD, 48, 5, 150, 5, 0.002});
        tuner.add_field({"INCREMENT_SCALE", &INCREMENT_SCALE, 1, 1, 10, 1, 0.002});

        // see terms
        tuner.add_field({"SEE_PAWN", &SEE_PAWN, 77, 20, 300, 5, 0.002});
        tuner.add_field({"SEE_KNIGHT", &SEE_KNIGHT, 299, 120, 500, 10, 0.002});
        tuner.add_field({"SEE_BISHOP", &SEE_BISHOP, 303, 120, 500, 10, 0.002});
        tuner.add_field({"SEE_ROOK", &SEE_ROOK, 504, 300, 700, 10, 0.002});
        tuner.add_field({"SEE_QUEEN", &SEE_QUEEN, 903, 500, 1300, 10, 0.002});
        tuner.add_field({"SEE_QUIET", &SEE_QUIET, 67, 5, 200, 3, 0.002});
        tuner.add_field({"SEE_CAPTURE", &SEE_CAPTURE, 32, 5, 200, 2, 0.002});
        tuner.add_field({"SEE_DEPTH", &SEE_DEPTH, 10, 1, 15, 1, 0.002});
        tuner.add_field({"QS_SEE_THRESHOLD", &QS_SEE_THRESHOLD, 9, 1, 100, 1, 0.002});

        // aspiration window terms
        tuner.add_field({"INITIAL_ASP_DELTA", &INITIAL_ASP_DELTA, 12, 1, 50, 1, 0.002});
        tuner.add_field({"ASP_MULTIPLIER", &ASP_MULTIPLIER, 1.3111, 1.0, 5.0, 0.25, 0.002});
        tuner.add_field({"MIN_ASP_DEPTH", &MIN_ASP_DEPTH, 4, 1, 10, 1, 0.002});

        // search terms
        tuner.add_field({"LMP_BASE", &LMP_BASE, 2, 2, 14, 1, 0.002});
        tuner.add_field({"RFP_MARGIN", &RFP_MARGIN, 72, 20, 350, 20, 0.002});
        tuner.add_field({"RAZOR_MARGIN", &RAZOR_MARGIN, 298, 100, 400, 15, 0.002});
        tuner.add_field({"NMP_BASE_REDUCTION", &NMP_BASE_REDUCTION, 5, 2, 10, 0.75, 0.002});
        tuner.add_field({"NMP_DEPTH", &NMP_DEPTH, 1, 1, 6, 0.85, 0.002});
        tuner.add_field({"RFP_DEPTH", &RFP_DEPTH, 6, 2, 10, 1, 0.002});
        tuner.add_field({"RAZOR_DEPTH", &RAZOR_DEPTH, 7, 2, 10, 1, 0.002});
        tuner.add_field({"IIR_DEPTH", &IIR_DEPTH, 4, 1, 8, 0.75, 0.002});
        tuner.add_field({"LMP_MULTIPLIER", &LMP_MULTIPLIER, 1, 1, 8, 0.75, 0.002});
        tuner.add_field({"LMR_DEPTH", &LMR_DEPTH, 4, 1, 8, 1, 0.002});
        tuner.add_field({"FP_BASE", &FP_BASE, 146, 20, 500, 10, 0.002});
        tuner.add_field({"FP_MULTIPLIER", &FP_MULTIPLIER, 118, 20, 400, 5, 0.002});
        tuner.add_field({"FP_DEPTH", &FP_DEPTH, 5, 1, 15, 1, 0.002});
        tuner.add_field({"NMP_DIVISOR", &NMP_DIVISOR, 9, 4, 17, 1, 0.002});
        tuner.add_field({"LMR_OFFSET", &LMR_OFFSET, 0.5137, 0.0, 4.0, 0.05, 0.002});
        tuner.add_field({"LMR_DIVISOR", &LMR_DIVISOR, 1.711, 0.5, 10.0, 0.1, 0.002});
    }
}
#endif