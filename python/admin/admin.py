#!/usr/bin/env python3

"""管理员统一入口。

默认交互式选择 config/*.toml 配置并执行管理操作。
自动化场景可通过环境变量 ADMIN_CONFIG 和 ADMIN_DRY_RUN 指定。
"""

from __future__ import annotations

import json
import os
import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent.parent))

from lib.operations import DeployRunner
from lib.toml_loader import load_toml, validate_deploy_config

PROJECT_ROOT = Path(__file__).resolve().parent.parent.parent
CONFIG_DIR = PROJECT_ROOT / "config"


def _discover_configs() -> list[Path]:
    configs = sorted(
        path for path in CONFIG_DIR.glob("*.toml")
        if path.name != "deploy_example.toml"
    )
    if configs:
        return configs
    return sorted(CONFIG_DIR.glob("*.toml"))


def _bool_from_env(value: str | None) -> bool | None:
    if value is None:
        return None
    normalized = value.strip().lower()
    if normalized in {"1", "true", "yes", "y", "on"}:
        return True
    if normalized in {"0", "false", "no", "n", "off"}:
        return False
    return None


def _choose_config() -> Path:
    env_config = os.environ.get("ADMIN_CONFIG")
    if env_config:
        path = Path(env_config)
        return path if path.is_absolute() else PROJECT_ROOT / path

    configs = _discover_configs()
    if not configs:
        raise FileNotFoundError("config 目录下没有可用的 TOML 配置文件")

    if not sys.stdin.isatty():
        preferred = CONFIG_DIR / "deploy.toml"
        if preferred.exists():
            return preferred
        return configs[0]

    print("可用配置文件：")
    for index, path in enumerate(configs, start=1):
        print(f"  {index}. {path.relative_to(PROJECT_ROOT)}")

    default_index = 1
    raw = input(f"选择配置 [默认 {default_index}]: ").strip()
    if not raw:
        return configs[default_index - 1]

    try:
        selected_index = int(raw)
    except ValueError as exc:
        raise ValueError("请输入配置序号") from exc

    if selected_index < 1 or selected_index > len(configs):
        raise ValueError("配置序号超出范围")

    return configs[selected_index - 1]


def _choose_dry_run() -> bool:
    env_value = _bool_from_env(os.environ.get("ADMIN_DRY_RUN"))
    if env_value is not None:
        return env_value

    if not sys.stdin.isatty():
        return True

    raw = input("先预演不写库？[Y/n]: ").strip().lower()
    return raw not in {"n", "no", "0", "false"}


def main() -> int:
    if len(sys.argv) > 1:
        print("提示：admin.py 已切换为配置选择入口，不再接收命令行参数。", file=sys.stderr)

    try:
        config_path = _choose_config()
        dry_run = _choose_dry_run()
    except Exception as exc:
        print(f"无法选择配置: {exc}", file=sys.stderr)
        return 1

    try:
        config = load_toml(config_path)
    except Exception as exc:
        print(f"加载配置失败: {exc}", file=sys.stderr)
        return 1

    warnings = validate_deploy_config(config)
    if warnings:
        print("配置校验发现问题:", file=sys.stderr)
        for warning in warnings:
            print(f"  - {warning}", file=sys.stderr)
        if any("缺少" in warning or "不合法" in warning for warning in warnings):
            print("配置未通过校验，终止执行。", file=sys.stderr)
            return 1

    mode_label = "预演" if dry_run else "写入"
    print(f"Deploy Runner [{mode_label}] {config_path.relative_to(PROJECT_ROOT)}", file=sys.stderr)
    print("=" * 56, file=sys.stderr)

    runner = DeployRunner(config, dry_run=dry_run)
    results = runner.run_all()
    runner.print_summary()
    print(json.dumps(results, ensure_ascii=False, indent=2))

    return 1 if runner.errors else 0


if __name__ == "__main__":
    raise SystemExit(main())
