#include "action/DocumentAction.h"

namespace Action {
    json DocumentAction::GetDocument(
        const std::string &product_code,
        const std::string &version,
        const std::string &channel)
    {
        if (!storage) {
            return {{"code", 500}, {"message", "storage not initialized"}, {"data", json::object()}};
        }

        auto document = storage->getDocument(product_code, version, channel);
        if (document.empty()) {
            return {
                {"code", 404},
                {"message", "release document not found"},
                {"data", {{"product_code", product_code}, {"requested_version", version}}}
            };
        }

        return {{"code", 0}, {"message", "ok"}, {"data", document}};
    }
}
