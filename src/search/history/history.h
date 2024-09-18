#pragma once

#include "../ss.h"
#include "defs.h"
#include "move.h"
#include "types.h"

#include "continuation_history.h"
#include "countermove_history.h"
#include "history_formulas.h"
#include "quiet_history.h"

namespace elixir {
    class History {
      public:
        History() { clear(); }
        ~History() = default;

        void clear();

        int get_history(move::Move &move, const search::SearchStack *ss) const {
            return 2 * quiet_history.get_quiet_history(move.get_from(), move.get_to()) +
                   (continuation_history.get_chs(move, ss - 1) +
                    continuation_history.get_chs(move, ss - 2) +
                    continuation_history.get_chs(move, ss - 4));
        }

        QuietHistory quiet_history;
        ContiuationHistory continuation_history;
        CounterMoveHistory countermove_history;
    };
}