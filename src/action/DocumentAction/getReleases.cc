#include "action/DocumentAction.h"

namespace Action {
    json DocumentAction::GetListReleases(const std::string &product_code){
        if (!storage) {
            return {{"code", 500}, {"message", "storage not initialized"}, {"data", json::object()}};
        }

        return {
            {"code", 0},
            {"message", "ok"},
            {"data", {{"items", storage->getALLReleases(product_code)}}}
        };
    }
}
