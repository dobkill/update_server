#include "action/ProductsAction.h"

namespace Action {
    json ProductsAction::GetRecentUpdates()
    {
        if (!storage) {
            return json::array();
        }

        return storage->getRecentUpdates();
    }
}
