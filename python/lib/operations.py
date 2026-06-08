#!/usr/bin/env python3

"""操作编排器 — 按依赖顺序调度各操作，收集并汇总执行结果。"""

from __future__ import annotations

import sys

from admin._asset import add_asset
from admin._channel import set_release_channel
from admin._cleanup import clean_temp_files
from admin._convert_rule import add_convert_rule
from admin._page import page_offline, page_publish, page_rollback
from admin._portfolio_project import upsert_portfolio_project
from admin._product import add_product
from admin._release import add_release
from admin._site_profile import set_site_profile
from admin._update_package import add_update_package


class DeployRunner:
    """读取 TOML 配置字典，按依赖顺序依次执行所有操作。"""

    def __init__(self, config: dict, dry_run: bool = False):
        self.config = config
        self.dry_run = dry_run
        self.results: list[dict] = []
        self.errors: list[dict] = []

    # ── 执行入口 ──────────────────────────────────────

    def run_all(self) -> list[dict]:
        """按顺序执行所有配置的操作。"""
        self._run_site_profile()
        self._run_products()
        self._run_portfolio_projects()
        self._run_releases()
        self._run_update_packages()
        self._run_convert_rules()
        self._run_page_operations()
        self._run_cleanup()
        return self.results

    # ── 内部执行方法 ───────────────────────────────────

    def _exec(self, section: str, func, params: dict) -> dict:
        """执行单个操作，处理 dry_run 和异常。"""
        label = f"{section}"
        if self.dry_run:
            result = {"section": section, "action": "dry_run", "params": params}
            self._print_step("🔍", section, "dry_run (预演)", "")
            self.results.append(result)
            return result

        try:
            result = func(params)
        except Exception as e:
            result = {"section": section, "action": "error", "reason": str(e)}
            self.errors.append(result)

        # 打印步骤结果
        action = result.get("action", "unknown")
        if action == "error":
            icon = "❌"
        elif action == "skipped":
            icon = "⏭️"
        else:
            icon = "✅"

        detail = self._format_detail(result)
        self._print_step(icon, section, action, detail)

        self.results.append(result)
        return result

    def _run_products(self) -> None:
        for p in self.config.get("product", []):
            self._exec("product", add_product, p)

    def _run_site_profile(self) -> None:
        profile = self.config.get("site_profile")
        if profile is not None:
            self._exec("site_profile", set_site_profile, profile)

    def _run_portfolio_projects(self) -> None:
        for item in self.config.get("portfolio_project", []):
            self._exec("portfolio_project", upsert_portfolio_project, item)

    def _run_releases(self) -> None:
        """release 包含内嵌的 asset 和 channel，需按顺序编排。"""
        for r in self.config.get("release", []):
            # 2a. 添加版本
            release_params = {
                k: v for k, v in r.items()
                if k not in ("asset", "channel")
            }
            res = self._exec("release", add_release, release_params)

            # 如果版本创建失败且非跳过，跳过该 release 的后续操作
            if res.get("action") == "error":
                continue

            # 2b. 添加全量包
            for a in r.get("asset", []):
                asset_params = {
                    "product_code": r["product_code"],
                    "version": r["version"],
                    **a,
                }
                self._exec("release.asset", add_asset, asset_params)

            # 2c. 设置渠道
            for ch in r.get("channel", []):
                # 如果 channel 块没有显式指定 platform/arch/package_type，
                # 从第一个 asset 继承（如果有）
                assets = r.get("asset", [])
                defaults = {}
                if assets:
                    defaults = {
                        "platform": assets[0].get("platform", "windows"),
                        "arch": assets[0].get("arch", "x64"),
                        "package_type": assets[0].get("package_type", "portable"),
                    }

                channel_params = {
                    "product_code": r["product_code"],
                    "version": r["version"],
                    **defaults,
                    **ch,
                }
                # 将 bool 转 int
                if isinstance(channel_params.get("is_latest"), bool):
                    channel_params["is_latest"] = int(channel_params["is_latest"])

                self._exec("release.channel", set_release_channel, channel_params)

    def _run_update_packages(self) -> None:
        for u in self.config.get("update_package", []):
            self._exec("update_package", add_update_package, u)

    def _run_convert_rules(self) -> None:
        for c in self.config.get("convert_rule", []):
            self._exec("convert_rule", add_convert_rule, c)

    def _run_page_operations(self) -> None:
        pp = self.config.get("page_publish")
        if pp is not None:
            self._exec("page_publish", page_publish, pp)

        po = self.config.get("page_offline")
        if po is not None:
            self._exec("page_offline", page_offline, po)

        pr = self.config.get("page_rollback")
        if pr is not None:
            self._exec("page_rollback", page_rollback, pr)

    def _run_cleanup(self) -> None:
        cl = self.config.get("cleanup")
        if cl is not None:
            self._exec("cleanup", clean_temp_files, cl)

    # ── 输出格式化 ─────────────────────────────────────

    @staticmethod
    def _print_step(icon: str, section: str, action: str, detail: str) -> None:
        """打印单步执行结果。"""
        line = f"  {icon} [{section}] {action}"
        if detail:
            line += f"  {detail}"
        print(line, file=sys.stderr)

    @staticmethod
    def _format_detail(result: dict) -> str:
        """从结果中提取关键信息用于显示。"""
        parts = []
        for key in ("product_id", "release_id", "asset_id", "channel_id",
                     "package_id", "rule_id", "product_code", "version",
                     "platform", "arch", "channel", "page_key", "removed",
                     "reason"):
            val = result.get(key)
            if val is not None:
                parts.append(f"{key}={val}")
        return "  ".join(parts)

    def print_summary(self) -> None:
        """打印执行摘要。"""
        total = len(self.results)
        created = sum(1 for r in self.results if r.get("action") not in ("error", "dry_run"))
        skipped = sum(1 for r in self.results if r.get("action") == "skipped")
        errors = len(self.errors)

        print("", file=sys.stderr)
        print("═" * 50, file=sys.stderr)
        if errors:
            print(f"⚠️  完成 {total} 步，{created} 成功，{skipped} 跳过，{errors} 错误", file=sys.stderr)
        else:
            print(f"✅ 全部完成 {total} 步（{created} 成功，{skipped} 跳过）", file=sys.stderr)
        print("═" * 50, file=sys.stderr)
