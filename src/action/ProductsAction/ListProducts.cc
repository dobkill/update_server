#include "action/ProductsAction.h"

namespace Action {
    json ProductsAction::ListProducts()
    {
        if (!storage) {
            return {{"code", 500}, {"message", "storage not initialized"}, {"data", {{"items", json::array()}}}};
        }

        return {
            {"code", 0},
            {"message", "ok"},
            {"data", {{"items", storage->getALLProducts()}}}
        };
    }
}
