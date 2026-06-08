PRAGMA journal_mode = WAL;
PRAGMA synchronous = NORMAL;
PRAGMA foreign_keys = ON;


-- =========================================================
-- 0. 迁移版本表
-- =========================================================
CREATE TABLE IF NOT EXISTS schema_migrations (
    version TEXT PRIMARY KEY,
    applied_at TEXT NOT NULL DEFAULT CURRENT_TIMESTAMP,
    note TEXT NOT NULL DEFAULT ''
);


-- =========================================================
-- 1. 站点配置表：站点信息配置
-- =========================================================
CREATE TABLE IF NOT EXISTS site_profile (
    id INTEGER PRIMARY KEY CHECK(id = 1),
    owner_name TEXT NOT NULL DEFAULT 'LIANG Y.',
    site_name TEXT NOT NULL DEFAULT 'Personal Software Lab',
    subtitle TEXT NOT NULL DEFAULT 'Self-built apps, tools, systems, and experiments.',
    hero_label TEXT NOT NULL DEFAULT 'DEVELOPER & BUILDER',
    hero_title TEXT NOT NULL DEFAULT 'Personal Software Lab',
    hero_description TEXT NOT NULL DEFAULT 'A collection of self-built apps, tools, systems, and experiments — crafted with code and curiosity.',
    github_url TEXT NOT NULL DEFAULT '',
    email TEXT NOT NULL DEFAULT '',
    resume_url TEXT NOT NULL DEFAULT '',
    linkedin_url TEXT NOT NULL DEFAULT '',
    twitter_url TEXT NOT NULL DEFAULT '',
    created_at TEXT NOT NULL DEFAULT CURRENT_TIMESTAMP,
    updated_at TEXT NOT NULL DEFAULT CURRENT_TIMESTAMP
);

-- =========================================================
-- 1. 产品表：一个软件产品本体
-- =========================================================
CREATE TABLE IF NOT EXISTS products (
    id INTEGER PRIMARY KEY AUTOINCREMENT,

    name TEXT NOT NULL,
    code TEXT NOT NULL UNIQUE,
    github_url TEXT NOT NULL DEFAULT '',
    description TEXT,
    icon_path TEXT,

    status TEXT NOT NULL DEFAULT 'active',

    created_at TEXT NOT NULL DEFAULT CURRENT_TIMESTAMP,
    updated_at TEXT NOT NULL DEFAULT CURRENT_TIMESTAMP,

    CHECK(status IN ('active', 'disabled', 'archived'))
);

-- =========================================================
-- 2. 作品集项目表：Web 展示用的软件案例
-- =========================================================
CREATE TABLE IF NOT EXISTS portfolio_projects (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    product_id INTEGER,
    slug TEXT NOT NULL UNIQUE,
    name TEXT NOT NULL,
    category TEXT NOT NULL,
    description TEXT NOT NULL DEFAULT '',
    long_description TEXT NOT NULL DEFAULT '',
    featured INTEGER NOT NULL DEFAULT 0,
    status TEXT NOT NULL DEFAULT 'Live',
    year TEXT NOT NULL DEFAULT '',
    platform TEXT NOT NULL DEFAULT '',
    role TEXT NOT NULL DEFAULT '',
    project_type TEXT NOT NULL DEFAULT '',
    cover_image_url TEXT NOT NULL DEFAULT '',
    hero_image_url TEXT NOT NULL DEFAULT '',
    tech_stack_json TEXT NOT NULL DEFAULT '[]',
    features_json TEXT NOT NULL DEFAULT '[]',
    screenshots_json TEXT NOT NULL DEFAULT '[]',
    architecture_json TEXT NOT NULL DEFAULT '[]',
    challenge TEXT NOT NULL DEFAULT '',
    solution TEXT NOT NULL DEFAULT '',
    result TEXT NOT NULL DEFAULT '',
    links_json TEXT NOT NULL DEFAULT '{}',
    visibility TEXT NOT NULL DEFAULT 'public',
    sort_order INTEGER NOT NULL DEFAULT 100,
    created_at TEXT NOT NULL DEFAULT CURRENT_TIMESTAMP,
    updated_at TEXT NOT NULL DEFAULT CURRENT_TIMESTAMP,
    FOREIGN KEY(product_id) REFERENCES products(id),
    CHECK(featured IN (0, 1)),
    CHECK(visibility IN ('public', 'hidden'))
);

INSERT INTO site_profile (
    id, owner_name, site_name, subtitle, hero_label, hero_title, hero_description,
    github_url, email, resume_url, linkedin_url, twitter_url
)
VALUES (
    1, 'LIANG Y.', 'Personal Software Lab', 'Self-built apps, tools, systems, and experiments.',
    'DEVELOPER & BUILDER', 'Personal Software Lab',
    'A collection of self-built apps, tools, systems, and experiments — crafted with code and curiosity.',
    '', '', '', '', ''
)
ON CONFLICT(id) DO NOTHING;

-- =========================================================
-- 3. 版本表：某个产品的某个版本
-- =========================================================
CREATE TABLE IF NOT EXISTS releases (
    id INTEGER PRIMARY KEY AUTOINCREMENT,

    product_id INTEGER NOT NULL,

    version TEXT NOT NULL,
    release_note TEXT,

    -- 软件内部数据格式版本，用于判断是否需要数据转换
    data_schema_version TEXT,

    -- draft: 草稿
    -- active: 可用
    -- deprecated: 不推荐
    -- yanked: 已撤回
    status TEXT NOT NULL DEFAULT 'draft',

    html_path TEXT,

    created_at TEXT NOT NULL DEFAULT CURRENT_TIMESTAMP,
    published_at TEXT,
    updated_at TEXT NOT NULL DEFAULT CURRENT_TIMESTAMP,

    FOREIGN KEY(product_id) REFERENCES products(id),

    UNIQUE(product_id, version),
    UNIQUE(id, product_id),

    CHECK(status IN ('draft', 'active', 'deprecated', 'yanked'))
);


-- =========================================================
-- 3. 全量包表：某个版本在某个平台、架构、包类型下的完整下载包
-- =========================================================
CREATE TABLE IF NOT EXISTS release_assets (
    id INTEGER PRIMARY KEY AUTOINCREMENT,

    product_id INTEGER NOT NULL,
    release_id INTEGER NOT NULL,

    platform TEXT NOT NULL DEFAULT 'windows',
    arch TEXT NOT NULL DEFAULT 'x64',
    package_type TEXT NOT NULL DEFAULT 'portable',

    file_path TEXT NOT NULL,
    file_size INTEGER NOT NULL,
    sha256 TEXT,
    md5 TEXT,

    status TEXT NOT NULL DEFAULT 'active',

    created_at TEXT NOT NULL DEFAULT CURRENT_TIMESTAMP,
    updated_at TEXT NOT NULL DEFAULT CURRENT_TIMESTAMP,

    FOREIGN KEY(product_id) REFERENCES products(id),
    FOREIGN KEY(release_id, product_id) REFERENCES releases(id, product_id),

    UNIQUE(release_id, platform, arch, package_type),

    CHECK(file_size >= 0),
    CHECK(sha256 IS NULL OR length(sha256) = 64),
    CHECK(md5 IS NULL OR length(md5) = 32),
    CHECK(status IN ('active', 'disabled', 'yanked'))
);


-- =========================================================
-- 4. 渠道发布表：控制 stable / beta / dev 的 latest 
-- =========================================================
CREATE TABLE IF NOT EXISTS release_channels (
    id INTEGER PRIMARY KEY AUTOINCREMENT,

    product_id INTEGER NOT NULL,
    release_id INTEGER NOT NULL,

    channel TEXT NOT NULL DEFAULT 'stable',

    platform TEXT NOT NULL DEFAULT 'windows',
    arch TEXT NOT NULL DEFAULT 'x64',
    package_type TEXT NOT NULL DEFAULT 'portable',

    is_latest INTEGER NOT NULL DEFAULT 0,


    status TEXT NOT NULL DEFAULT 'active',

    created_at TEXT NOT NULL DEFAULT CURRENT_TIMESTAMP,
    updated_at TEXT NOT NULL DEFAULT CURRENT_TIMESTAMP,

    FOREIGN KEY(product_id) REFERENCES products(id),
    FOREIGN KEY(release_id, product_id) REFERENCES releases(id, product_id),
    FOREIGN KEY(release_id, platform, arch, package_type)
        REFERENCES release_assets(release_id, platform, arch, package_type),

    UNIQUE(release_id, channel, platform, arch, package_type),

    CHECK(is_latest IN (0, 1)),
    CHECK(status IN ('active', 'disabled', 'yanked'))
);


-- 同一个产品 + 渠道 + 平台 + 架构 + 包类型，只允许一个 latest
CREATE UNIQUE INDEX IF NOT EXISTS idx_release_channels_latest
ON release_channels(product_id, channel, platform, arch, package_type)
WHERE is_latest = 1 AND status = 'active';


-- =========================================================
-- 5. 增量包表：从某个版本升级到某个版本
-- =========================================================
CREATE TABLE IF NOT EXISTS update_packages (
    id INTEGER PRIMARY KEY AUTOINCREMENT,

    product_id INTEGER NOT NULL,

    source_release_id INTEGER NOT NULL,
    target_release_id INTEGER NOT NULL,

    platform TEXT NOT NULL DEFAULT 'windows',
    arch TEXT NOT NULL DEFAULT 'x64',
    package_type TEXT NOT NULL DEFAULT 'portable',

    update_type TEXT NOT NULL DEFAULT 'incremental',

    file_path TEXT NOT NULL,
    file_size INTEGER NOT NULL,
    sha256 TEXT NOT NULL,

    status TEXT NOT NULL DEFAULT 'active',

    created_at TEXT NOT NULL DEFAULT CURRENT_TIMESTAMP,
    updated_at TEXT NOT NULL DEFAULT CURRENT_TIMESTAMP,

    FOREIGN KEY(product_id) REFERENCES products(id),

    UNIQUE(source_release_id, target_release_id, platform, arch, package_type),

    CHECK(source_release_id <> target_release_id),
    CHECK(update_type IN ('incremental', 'patch')),
    CHECK(file_size >= 0),
    CHECK(length(sha256) = 64),
    CHECK(status IN ('active', 'disabled', 'yanked'))
);


-- =========================================================
-- 6. 数据转换规则表：定义旧数据格式如何转到新数据格式
-- =========================================================
CREATE TABLE IF NOT EXISTS convert_rules (
    id INTEGER PRIMARY KEY AUTOINCREMENT,

    product_id INTEGER NOT NULL,

    source_data_schema_version TEXT NOT NULL,
    target_data_schema_version TEXT NOT NULL,

    script_path TEXT NOT NULL,
    cost_time  INTEGER NOT NULL DEFAULT 5,
    script_sha256 TEXT,

    status TEXT NOT NULL DEFAULT 'active',

    created_at TEXT NOT NULL DEFAULT CURRENT_TIMESTAMP,
    updated_at TEXT NOT NULL DEFAULT CURRENT_TIMESTAMP,

    FOREIGN KEY(product_id) REFERENCES products(id),

    UNIQUE(product_id, source_data_schema_version, target_data_schema_version),

    CHECK(source_data_schema_version <> target_data_schema_version),
    CHECK(script_sha256 IS NULL OR length(script_sha256) = 64),
    CHECK(status IN ('active', 'disabled', 'deprecated'))
);


-- =========================================================
-- 7. 数据转换任务表：记录某一次具体转换任务
-- =========================================================
CREATE TABLE IF NOT EXISTS convert_tasks (
    id INTEGER PRIMARY KEY AUTOINCREMENT,

    task_id TEXT NOT NULL UNIQUE,

    product_id INTEGER NOT NULL,

    convert_rule_id INTEGER,

    source_release_id INTEGER,
    target_release_id INTEGER,

    source_data_schema_version TEXT NOT NULL,
    target_data_schema_version TEXT NOT NULL,

    status TEXT NOT NULL DEFAULT 'running',

    input_file_path TEXT NOT NULL,
    input_file_sha256 TEXT,

    output_file_path TEXT,
    output_file_sha256 TEXT,

    log_file_path TEXT,
    error_message TEXT,
    retry_count INTEGER NOT NULL DEFAULT 0,

    created_at TEXT NOT NULL DEFAULT CURRENT_TIMESTAMP,
    updated_at TEXT NOT NULL DEFAULT CURRENT_TIMESTAMP,


    FOREIGN KEY(product_id) REFERENCES products(id),
    FOREIGN KEY(convert_rule_id) REFERENCES convert_rules(id),
    FOREIGN KEY(source_release_id, product_id) REFERENCES releases(id, product_id),
    FOREIGN KEY(target_release_id, product_id) REFERENCES releases(id, product_id),

    CHECK(status IN ('pending', 'running', 'success', 'failed', 'canceled')),
    CHECK(retry_count >= 0),
    CHECK(input_file_sha256 IS NULL OR length(input_file_sha256) = 64),
    CHECK(output_file_sha256 IS NULL OR length(output_file_sha256) = 64)
);


-- =========================================================
-- 常用索引
-- =========================================================

CREATE INDEX IF NOT EXISTS idx_products_code
ON products(code);

CREATE INDEX IF NOT EXISTS idx_portfolio_projects_public_order
ON portfolio_projects(visibility, sort_order, id);

CREATE INDEX IF NOT EXISTS idx_portfolio_projects_category
ON portfolio_projects(category, visibility, sort_order);

CREATE INDEX IF NOT EXISTS idx_releases_product_version
ON releases(product_id, version);

CREATE INDEX IF NOT EXISTS idx_releases_product_status
ON releases(product_id, status);

CREATE INDEX IF NOT EXISTS idx_release_assets_lookup
ON release_assets(product_id, release_id, platform, arch, package_type, status);

CREATE INDEX IF NOT EXISTS idx_release_channels_check_update
ON release_channels(product_id, channel, platform, arch, package_type, status, is_latest);

CREATE INDEX IF NOT EXISTS idx_update_packages_lookup
ON update_packages(product_id, source_release_id, target_release_id, platform, arch, package_type, status);

CREATE INDEX IF NOT EXISTS idx_convert_rules_schema
ON convert_rules(product_id, source_data_schema_version, target_data_schema_version, status);

CREATE INDEX IF NOT EXISTS idx_convert_tasks_task_id
ON convert_tasks(task_id);

CREATE INDEX IF NOT EXISTS idx_convert_tasks_status
ON convert_tasks(status, created_at);

CREATE INDEX IF NOT EXISTS idx_convert_tasks_product
ON convert_tasks(product_id, created_at);


-- =========================================================
-- updated_at 自动更新时间触发器
-- =========================================================

CREATE TRIGGER IF NOT EXISTS trg_products_updated_at
AFTER UPDATE ON products
FOR EACH ROW
WHEN NEW.updated_at = OLD.updated_at
BEGIN
    UPDATE products
    SET updated_at = CURRENT_TIMESTAMP
    WHERE id = OLD.id;
END;

CREATE TRIGGER IF NOT EXISTS trg_site_profile_updated_at
AFTER UPDATE ON site_profile
FOR EACH ROW
WHEN NEW.updated_at = OLD.updated_at
BEGIN
    UPDATE site_profile
    SET updated_at = CURRENT_TIMESTAMP
    WHERE id = OLD.id;
END;

CREATE TRIGGER IF NOT EXISTS trg_portfolio_projects_updated_at
AFTER UPDATE ON portfolio_projects
FOR EACH ROW
WHEN NEW.updated_at = OLD.updated_at
BEGIN
    UPDATE portfolio_projects
    SET updated_at = CURRENT_TIMESTAMP
    WHERE id = OLD.id;
END;

CREATE TRIGGER IF NOT EXISTS trg_releases_updated_at
AFTER UPDATE ON releases
FOR EACH ROW
WHEN NEW.updated_at = OLD.updated_at
BEGIN
    UPDATE releases
    SET updated_at = CURRENT_TIMESTAMP
    WHERE id = OLD.id;
END;

CREATE TRIGGER IF NOT EXISTS trg_release_assets_updated_at
AFTER UPDATE ON release_assets
FOR EACH ROW
WHEN NEW.updated_at = OLD.updated_at
BEGIN
    UPDATE release_assets
    SET updated_at = CURRENT_TIMESTAMP
    WHERE id = OLD.id;
END;

CREATE TRIGGER IF NOT EXISTS trg_release_channels_updated_at
AFTER UPDATE ON release_channels
FOR EACH ROW
WHEN NEW.updated_at = OLD.updated_at
BEGIN
    UPDATE release_channels
    SET updated_at = CURRENT_TIMESTAMP
    WHERE id = OLD.id;
END;

CREATE TRIGGER IF NOT EXISTS trg_update_packages_updated_at
AFTER UPDATE ON update_packages
FOR EACH ROW
WHEN NEW.updated_at = OLD.updated_at
BEGIN
    UPDATE update_packages
    SET updated_at = CURRENT_TIMESTAMP
    WHERE id = OLD.id;
END;

CREATE TRIGGER IF NOT EXISTS trg_convert_rules_updated_at
AFTER UPDATE ON convert_rules
FOR EACH ROW
WHEN NEW.updated_at = OLD.updated_at
BEGIN
    UPDATE convert_rules
    SET updated_at = CURRENT_TIMESTAMP
    WHERE id = OLD.id;
END;

CREATE TRIGGER IF NOT EXISTS trg_convert_tasks_updated_at
AFTER UPDATE ON convert_tasks
FOR EACH ROW
WHEN NEW.updated_at = OLD.updated_at
BEGIN
    UPDATE convert_tasks
    SET updated_at = CURRENT_TIMESTAMP
    WHERE id = OLD.id;
END;
