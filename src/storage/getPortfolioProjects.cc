#include "storage/storage.h"

#include <string>

namespace Storage
{
    namespace
    {
        json parseJsonField(const json &row, const std::string &key, const json &fallback)
        {
            if (!row.contains(key) || row[key].is_null())
            {
                return fallback;
            }

            try
            {
                if (row[key].is_string())
                {
                    const auto raw = row[key].get<std::string>();
                    if (raw.empty())
                    {
                        return fallback;
                    }
                    return json::parse(raw);
                }
                return row[key];
            }
            catch (const json::parse_error &)
            {
                return fallback;
            }
        }

        json normalizeProject(json row)
        {
            row["featured"] = row.value("featured", 0) == 1;
            row["techStack"] = parseJsonField(row, "tech_stack_json", json::array());
            row["features"] = parseJsonField(row, "features_json", json::array());
            row["screenshots"] = parseJsonField(row, "screenshots_json", json::array());
            row["architecture"] = parseJsonField(row, "architecture_json", json::array());
            row["links"] = parseJsonField(row, "links_json", json::object());

            row.erase("tech_stack_json");
            row.erase("features_json");
            row.erase("screenshots_json");
            row.erase("architecture_json");
            row.erase("links_json");
            return row;
        }

        std::string projectSelect()
        {
            return
                "SELECT "
                "pp.slug, "
                "pp.name, "
                "pp.category, "
                "pp.description, "
                "pp.long_description AS longDescription, "
                "pp.featured, "
                "pp.status, "
                "pp.year, "
                "pp.platform, "
                "pp.role, "
                "pp.project_type AS type, "
                "pp.cover_image_url AS coverImageUrl, "
                "pp.hero_image_url AS heroImageUrl, "
                "pp.tech_stack_json, "
                "pp.features_json, "
                "pp.screenshots_json, "
                "pp.architecture_json, "
                "pp.challenge, "
                "pp.solution, "
                "pp.result, "
                "pp.links_json, "
                "pp.sort_order AS sortOrder, "
                "p.code AS productCode "
                "FROM portfolio_projects pp "
                "LEFT JOIN products p ON p.id = pp.product_id ";
        }
    }

    json Storage_SQL::getPortfolioProjectList()
    {
        auto rows = queryRows(
            projectSelect() +
            "WHERE pp.visibility = 'public' "
            "ORDER BY pp.sort_order ASC, pp.id ASC;");

        json items = json::array();
        for (auto &row : rows)
        {
            items.push_back(normalizeProject(row));
        }
        return items;
    }

    json Storage_SQL::getFeaturedPortfolioProjects()
    {
        auto rows = queryRows(
            projectSelect() +
            "WHERE pp.visibility = 'public' "
            "  AND pp.featured = 1 "
            "ORDER BY pp.sort_order ASC, pp.id ASC;");

        json items = json::array();
        for (auto &row : rows)
        {
            items.push_back(normalizeProject(row));
        }
        return items;
    }

    json Storage_SQL::getPortfolioProjectDetail(const std::string &slug)
    {
        auto row = queryOne(
            projectSelect() +
            "WHERE pp.visibility = 'public' "
            "  AND pp.slug = ? COLLATE NOCASE "
            "LIMIT 1;",
            {slug});

        if (row.empty())
        {
            return json::object();
        }

        return normalizeProject(row);
    }
}
