PRAGMA journal_mode = WAL;
PRAGMA synchronous = NORMAL;
PRAGMA foreign_keys = ON;

-- =========================================================
-- schema_migrations
-- =========================================================
CREATE TABLE IF NOT EXISTS schema_migrations (
    version TEXT PRIMARY KEY,
    applied_at TEXT NOT NULL DEFAULT CURRENT_TIMESTAMP,
    note TEXT NOT NULL DEFAULT ''
);

INSERT INTO schema_migrations (version, note)
VALUES ('20260719_init_showcase', 'personal project showcase schema')
ON CONFLICT(version) DO NOTHING;

-- =========================================================
-- site_profile
-- =========================================================
CREATE TABLE IF NOT EXISTS site_profile (
    id INTEGER PRIMARY KEY CHECK(id = 1),
    owner_name TEXT NOT NULL DEFAULT 'Xiang Y.',
    site_name TEXT NOT NULL DEFAULT 'Personal Software Lab',
    subtitle TEXT NOT NULL DEFAULT 'Self-built apps, tools, systems, and experiments.',
    hero_label TEXT NOT NULL DEFAULT 'DEVELOPER & BUILDER',
    hero_title TEXT NOT NULL DEFAULT 'Personal Software Lab',
    hero_description TEXT NOT NULL DEFAULT 'A collection of self-built apps, tools, systems, and experiments.',
    github_url TEXT NOT NULL DEFAULT '',
    email TEXT NOT NULL DEFAULT '',
    resume_url TEXT NOT NULL DEFAULT '',
    linkedin_url TEXT NOT NULL DEFAULT '',
    twitter_url TEXT NOT NULL DEFAULT '',
    created_at TEXT NOT NULL DEFAULT CURRENT_TIMESTAMP,
    updated_at TEXT NOT NULL DEFAULT CURRENT_TIMESTAMP
);

INSERT INTO site_profile (
    id, owner_name, site_name, subtitle, hero_label, hero_title, hero_description,
    github_url, email, resume_url, linkedin_url, twitter_url
)
VALUES (
    1, 'Xiang Y.', 'Personal Software Lab', 'Self-built apps, tools, systems, and experiments.',
    'DEVELOPER & BUILDER', 'Personal Software Lab',
    'A collection of self-built apps, tools, systems, and experiments.',
    '', '', '', '', ''
)
ON CONFLICT(id) DO NOTHING;

-- =========================================================
-- projects
-- =========================================================
CREATE TABLE IF NOT EXISTS projects (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    slug TEXT NOT NULL UNIQUE,
    name TEXT NOT NULL,
    category TEXT NOT NULL DEFAULT '',
    summary TEXT NOT NULL DEFAULT '',
    description TEXT NOT NULL DEFAULT '',
    status TEXT NOT NULL DEFAULT 'active',
    year TEXT NOT NULL DEFAULT '',
    role TEXT NOT NULL DEFAULT '',
    platform TEXT NOT NULL DEFAULT '',
    project_type TEXT NOT NULL DEFAULT '',
    challenge TEXT NOT NULL DEFAULT '',
    solution TEXT NOT NULL DEFAULT '',
    result TEXT NOT NULL DEFAULT '',
    cover_asset_key TEXT NOT NULL DEFAULT '',
    hero_asset_key TEXT NOT NULL DEFAULT '',
    tech_stack_json TEXT NOT NULL DEFAULT '[]',
    features_json TEXT NOT NULL DEFAULT '[]',
    screenshots_json TEXT NOT NULL DEFAULT '[]',
    architecture_json TEXT NOT NULL DEFAULT '[]',
    links_json TEXT NOT NULL DEFAULT '{}',
    featured INTEGER NOT NULL DEFAULT 0,
    visibility TEXT NOT NULL DEFAULT 'public',
    sort_order INTEGER NOT NULL DEFAULT 100,
    created_at TEXT NOT NULL DEFAULT CURRENT_TIMESTAMP,
    updated_at TEXT NOT NULL DEFAULT CURRENT_TIMESTAMP,

    CHECK(featured IN (0, 1)),
    CHECK(visibility IN ('public', 'hidden')),
    CHECK(status IN ('active', 'archived', 'draft'))
);

-- =========================================================
-- project_pages
-- =========================================================
CREATE TABLE IF NOT EXISTS project_pages (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    project_id INTEGER NOT NULL,
    slug TEXT NOT NULL,
    title TEXT NOT NULL,
    summary TEXT NOT NULL DEFAULT '',
    html_asset_key TEXT NOT NULL DEFAULT '',
    sort_order INTEGER NOT NULL DEFAULT 100,
    visibility TEXT NOT NULL DEFAULT 'public',
    created_at TEXT NOT NULL DEFAULT CURRENT_TIMESTAMP,
    updated_at TEXT NOT NULL DEFAULT CURRENT_TIMESTAMP,

    FOREIGN KEY(project_id) REFERENCES projects(id) ON DELETE CASCADE,
    UNIQUE(project_id, slug),
    CHECK(visibility IN ('public', 'hidden'))
);

-- =========================================================
-- project_assets
-- =========================================================
CREATE TABLE IF NOT EXISTS project_assets (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    project_id INTEGER,
    asset_key TEXT NOT NULL UNIQUE,
    original_name TEXT NOT NULL DEFAULT '',
    content_type TEXT NOT NULL DEFAULT '',
    file_size INTEGER NOT NULL DEFAULT 0,
    sha256 TEXT NOT NULL DEFAULT '',
    kind TEXT NOT NULL DEFAULT 'file',
    created_at TEXT NOT NULL DEFAULT CURRENT_TIMESTAMP,
    updated_at TEXT NOT NULL DEFAULT CURRENT_TIMESTAMP,

    FOREIGN KEY(project_id) REFERENCES projects(id) ON DELETE CASCADE,
    CHECK(kind IN ('image', 'html', 'file', 'video'))
);

-- =========================================================
-- indexes
-- =========================================================
CREATE INDEX IF NOT EXISTS idx_projects_public_order
ON projects(visibility, sort_order, id);

CREATE INDEX IF NOT EXISTS idx_projects_category
ON projects(category, visibility, sort_order);

CREATE INDEX IF NOT EXISTS idx_project_pages_project
ON project_pages(project_id, visibility, sort_order);

CREATE INDEX IF NOT EXISTS idx_project_assets_project
ON project_assets(project_id);

CREATE INDEX IF NOT EXISTS idx_project_assets_key
ON project_assets(asset_key);

-- =========================================================
-- updated_at triggers
-- =========================================================
CREATE TRIGGER IF NOT EXISTS trg_site_profile_updated_at
AFTER UPDATE ON site_profile
FOR EACH ROW
WHEN NEW.updated_at = OLD.updated_at
BEGIN
    UPDATE site_profile SET updated_at = CURRENT_TIMESTAMP WHERE id = OLD.id;
END;

CREATE TRIGGER IF NOT EXISTS trg_projects_updated_at
AFTER UPDATE ON projects
FOR EACH ROW
WHEN NEW.updated_at = OLD.updated_at
BEGIN
    UPDATE projects SET updated_at = CURRENT_TIMESTAMP WHERE id = OLD.id;
END;

CREATE TRIGGER IF NOT EXISTS trg_project_pages_updated_at
AFTER UPDATE ON project_pages
FOR EACH ROW
WHEN NEW.updated_at = OLD.updated_at
BEGIN
    UPDATE project_pages SET updated_at = CURRENT_TIMESTAMP WHERE id = OLD.id;
END;

CREATE TRIGGER IF NOT EXISTS trg_project_assets_updated_at
AFTER UPDATE ON project_assets
FOR EACH ROW
WHEN NEW.updated_at = OLD.updated_at
BEGIN
    UPDATE project_assets SET updated_at = CURRENT_TIMESTAMP WHERE id = OLD.id;
END;
