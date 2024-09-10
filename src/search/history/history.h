#pragma once

#include "defs.h"
#include "move.h"
#include "types.h"
#include "../ss.h"

#include "quiet_history.h"
#include "continuation_history.h"
#include "countermove_history.h"
#include "history_formulas.h"

namespace elixir {
    class History {
      public:
        History()  {
            clear();
        }
        ~History() = default;

        void clear();

        int get_history(move::Move& move, const search::SearchStack *ss) const {
          return 2 * quiet_history.get_quiet_history(move.get_from(), move.get_to()) + (continuation_history.get_chs(move, ss - 1) + continuation_history.get_chs(move, ss - 2));
        }

        QuietHistory quiet_history;
        ContiuationHistory continuation_history;
        CounterMoveHistory countermove_history;
    };
}