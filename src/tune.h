#pragma once
#ifdef USE_TUNE

#include <iostream>
#include <variant>
#include <vector>

#include "types.h"
#include "defs.h"
#include "evaluate.h"
#include "history.h"
#endif

namespace elixir {
    extern int DEFAULT_MOVESTOGO;
    extern int DEFAULT_MOVE_OVERHEAD;
    extern int INCREMENT_SCALE;
}

#ifdef USE_TUNE
namespace elixir::tune {
    struct TunerField {
        std::string name;
        std::variant<int*, double*> value;
        std::variant<int, double> default_value;
        std::variant<int, double> min_value;
        std::variant<int, double> max_value;
        double step;
        double lr;
    };
    class Tuner {
        public:
            Tuner() = default;
            void add_field(TunerField field) ;
            void print_info();
            void print_current();
        private:
            std::vector<TunerField> fields;
    };

    void init_tune();

        extern Tuner tuner;    
}
#endif