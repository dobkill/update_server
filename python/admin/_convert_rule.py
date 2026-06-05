#!/usr/bin/env python3

"""数据转换规则操作核心业务逻辑。"""

from __future__ import annotations

import hashlib
from pathlib import Path

from lib.db import execute, get_product_id_by_code, query_one

_PROJECT_ROOT = Path(__file__).resolve().parent.parent.parent


def _compute_sha256(file_path: Path) -> str:
    h = hashlib.sha256()
    with open(file_path, "rb") as f:
        for chunk in iter(lambda: f.read(8192), b""):
            h.update(chunk)
    return h.hexdigest()


def add_convert_rule(params: dict) -> dict:
    """添加数据转换规则。

    params:
        product_code: str       必填
        source_schema: str      必填
        target_schema: str      必填
        script_path: str        必填
        cost_time: int          可选，默认 5
        script_sha256: str      可选
        auto_hash: bool         可选
        status: str             可选，默认 active
    """
    product_code = params["product_code"]
    source_schema = params["source_schema"]
    target_schema = params["target_schema"]
    script_path_str = params["script_path"]
    cost_time = params.get("cost_time", 5)
    status = params.get("status", "active")

    # 查找产品 ID
    try:
        product_id = get_product_id_by_code(product_code)
    except ValueError as e:
        return {"section": "convert_rule", "action": "error", "reason": str(e)}

    # 校验不重复
    existing = query_one(
        """
        SELECT id FROM convert_rules
        WHERE product_id = ? AND source_data_schema_version = ? AND target_data_schema_version = ?
        """,
        (product_id, source_schema, target_schema),
    )
    if existing is not None:
        return {
            "section": "convert_rule",
            "action": "skipped",
            "reason": f"转换规则已存在 (id={existing['id']})",
            "rule_id": existing["id"],
            "product_code": product_code,
            "source_schema": source_schema,
            "target_schema": target_schema,
        }

    # 解析脚本路径
    script_path = Path(script_path_str)
    abs_script_path = script_path if script_path.is_absolute() else _PROJECT_ROOT / script_path

    # 脚本 SHA256
    script_sha256 = params.get("script_sha256")
    auto_hash = params.get("auto_hash", False)
    if auto_hash and script_sha256 is None:
        if not abs_script_path.exists():
            return {"section": "convert_rule", "action": "error", "reason": f"脚本文件不存在: {abs_script_path}"}
        script_sha256 = _compute_sha256(abs_script_path)

    cur = execute(
        """
        INSERT INTO convert_rules (product_id, source_data_schema_version, target_data_schema_version, script_path, cost_time, script_sha256, status)
        VALUES (?, ?, ?, ?, ?, ?, ?)
        """,
        (product_id, source_schema, target_schema, script_path_str, cost_time, script_sha256, status),
    )

    rule_id = cur.lastrowid
    return {
        "section": "convert_rule",
        "action": "created",
        "rule_id": rule_id,
        "product_code": product_code,
        "source_schema": source_schema,
        "target_schema": target_schema,
        "script_path": script_path_str,
        "cost_time": cost_time,
        "script_sha256": script_sha256,
    }
