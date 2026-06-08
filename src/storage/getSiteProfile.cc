#include "storage/storage.h"

namespace Storage {
    json Storage_SQL::getSiteProfile()
    {
        auto profile = queryOne(
            "SELECT "
            "owner_name AS ownerName, "
            "site_name AS siteName, "
            "subtitle, "
            "hero_label AS heroLabel, "
            "hero_title AS heroTitle, "
            "hero_description AS heroDescription, "
            "github_url AS githubUrl, "
            "email, "
            "resume_url AS resumeUrl, "
            "linkedin_url AS linkedinUrl, "
            "twitter_url AS twitterUrl "
            "FROM site_profile "
            "WHERE id = 1 "
            "LIMIT 1;");

        if (profile.empty())
        {
            return {
                {"ownerName", "Xiang Y."},
                {"siteName", "Personal Software Lab"},
                {"subtitle", "Self-built apps, tools, systems, and experiments."},
                {"heroLabel", "DEVELOPER & BUILDER"},
                {"heroTitle", "Personal Software Lab"},
                {"heroDescription", "A collection of self-built apps, tools, systems, and experiments — crafted with code and curiosity."},
                {"githubUrl", ""},
                {"email", ""},
                {"resumeUrl", ""},
                {"linkedinUrl", ""},
                {"twitterUrl", ""}
            };
        }

        return profile;
    }
}
