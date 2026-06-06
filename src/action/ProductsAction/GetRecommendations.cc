#include "action/ProductsAction.h"

namespace Action {
    json ProductsAction::GetRecommendations()
    {
        if (!storage) {
            return json::array();
        }

        return storage->getRecommendations();
    }
}
