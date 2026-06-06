#include "action/ProductsAction.h"

namespace Action {
    json ProductsAction::GetPortfolioHome()
    {
        if (!storage) {
            return {{"code", 500}, {"message", "storage not initialized"}, {"data", json::object()}};
        }

        json profile = GetSiteProfile();
        json recommendations = GetRecommendations();
        json products = storage->getALLProducts();
        json recent_updates = GetRecentUpdates();
        json future_directions = GetFutureDirections();

        if ((!recommendations.is_array() || recommendations.empty()) && products.is_array() && !products.empty()) {
            recommendations = json::array({products.front()});
        }

        return {
            {"code", 0},
            {"message", "ok"},
            {"data", {
                {"profile", profile},
                {"recommendations", recommendations},
                {"products", products},
                {"recent_updates", recent_updates},
                {"future_directions", future_directions}
            }}
        };
    }
}
