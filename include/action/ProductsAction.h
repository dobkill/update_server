
#pragma once
#include <nlohmann/json.hpp>
#include <memory>
#include <string>

#include "storage/storage.h"

namespace Action {
    using json = nlohmann::json;

    class ProductsAction {
    public:
        json GetPortfolioHome();
        json ListProducts();
    private:
        json GetRecommendations();
        json GetFutureDirections();
        json GetSiteProfile();
        json GetRecentUpdates();
    private:
        std::shared_ptr<Storage::Storage_SQL> storage = Storage::Storage_SQL::Instance();
    };
}
