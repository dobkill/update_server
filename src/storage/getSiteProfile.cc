#include "storage/storage.h"

namespace Storage {
    json Storage_SQL::getSiteProfile()
    {
        auto profile = queryOne(
            "SELECT "
            "site_name, subtitle, github_url, email "
            "FROM site_profile "
            "WHERE id = 1 "
            "LIMIT 1;");

        if (profile.empty())
        {
            return {
                {"site_name", "YXX Works"},
                {"subtitle", "产品、插件与创作实验"},
                {"github_url", ""},
                {"email", ""}
            };
        }

        return profile;
    }
}
