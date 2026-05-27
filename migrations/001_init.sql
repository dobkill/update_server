PRAGMA journal_mode=WAL;
PRAGMA synchronous=NORMAL;
PRAGMA foreign_keys=ON;

CREATE TABLE IF NOT EXISTS app_versions (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    version TEXT NOT NULL,
    platform TEXT NOT NULL,
    arch TEXT NOT NULL DEFAULT 'x64',
    channel TEXT NOT NULL DEFAULT 'stable',
    package_type TEXT NOT NULL,
    base_version TEXT,
    file_path TEXT NOT NULL,
    file_size INTEGER NOT NULL,
    sha256 TEXT NOT NULL,
    release_note TEXT,
    is_latest INTEGER NOT NULL DEFAULT 0,
    force_update INTEGER NOT NULL DEFAULT 0,
    status TEXT NOT NULL DEFAULT 'active',
    created_at TEXT NOT NULL,
    updated_at TEXT NOT NULL
);

CREATE TABLE IF NOT EXISTS convert_tasks (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    task_id TEXT NOT NULL UNIQUE,
    source_version TEXT NOT NULL,
    target_version TEXT NOT NULL,
    input_file_path TEXT NOT NULL,
    output_file_path TEXT,
    log_file_path TEXT,
    status TEXT NOT NULL,
    error_message TEXT,
    created_at TEXT NOT NULL,
    updated_at TEXT NOT NULL,
    expired_at TEXT
);

CREATE TABLE IF NOT EXISTS page_definitions (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    page_key TEXT NOT NULL,
    route TEXT NOT NULL,
    title TEXT NOT NULL,
    manifest_path TEXT NOT NULL,
    page_version TEXT NOT NULL,
    status TEXT NOT NULL DEFAULT 'draft',
    created_by TEXT,
    created_at TEXT NOT NULL,
    updated_at TEXT NOT NULL,
    UNIQUE(page_key, page_version)
);

CREATE UNIQUE INDEX IF NOT EXISTS idx_page_active_key
ON page_definitions(page_key)
WHERE status = 'active';

CREATE UNIQUE INDEX IF NOT EXISTS idx_page_active_route
ON page_definitions(route)
WHERE status = 'active';

CREATE TABLE IF NOT EXISTS page_publish_records (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    page_key TEXT NOT NULL,
    page_version TEXT NOT NULL,
    action TEXT NOT NULL,
    operator TEXT,
    detail TEXT,
    created_at TEXT NOT NULL
);

CREATE TABLE IF NOT EXISTS download_logs (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    version TEXT,
    file_path TEXT,
    client_ip TEXT,
    user_agent TEXT,
    created_at TEXT NOT NULL
);

CREATE TABLE IF NOT EXISTS audit_logs (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    operator TEXT,
    action TEXT NOT NULL,
    target_type TEXT,
    target_id TEXT,
    detail TEXT,
    created_at TEXT NOT NULL
);
