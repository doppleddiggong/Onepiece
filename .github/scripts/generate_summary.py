#!/usr/bin/env python3
"""
HonKit SUMMARY.md 자동 생성 스크립트
Documents 폴더 구조를 스캔하여 SUMMARY.md를 자동으로 생성합니다.
"""

import os
import re
from pathlib import Path
from datetime import datetime
from typing import List, Dict, Tuple

class SummaryGenerator:
    def __init__(self, base_dir: str = "Documents"):
        self.base_dir = Path(base_dir)
        self.honkit_dir = self.base_dir  # HonKit root는 Documents
        self.devlog_dir = self.base_dir / "DevLog"
        self.planning_dir = self.base_dir / "Planning"

    def scan_devlog_files(self) -> Dict[str, List[Path]]:
        """DevLog 폴더의 파일들을 카테고리별로 스캔"""
        result = {
            "agent": [],
            "daily": [],
            "weekly": [],
            "monthly": [],
            "meta": []
        }

        # Agent Logs
        agent_log_dir = self.devlog_dir / "AgentLog"
        if agent_log_dir.exists():
            for user_dir in sorted(agent_log_dir.iterdir()):
                if user_dir.is_dir():
                    for md_file in sorted(user_dir.glob("*.md"), reverse=True):
                        result["agent"].append(md_file)

        # Daily Logs
        daily_dir = self.devlog_dir / "Daily"
        if daily_dir.exists():
            daily_files = sorted(daily_dir.glob("*.md"), reverse=True)
            result["daily"] = daily_files

        # Weekly Logs
        weekly_dir = self.devlog_dir / "Weekly"
        if weekly_dir.exists():
            weekly_files = sorted(weekly_dir.glob("*.md"), reverse=True)
            result["weekly"] = weekly_files

        # Monthly Logs
        monthly_dir = self.devlog_dir / "Monthly"
        if monthly_dir.exists():
            monthly_files = sorted(monthly_dir.glob("*.md"), reverse=True)
            result["monthly"] = monthly_files

        # Meta files (root level)
        meta_files = ["_Last30Summary.md", "IMPLEMENTATION_SUMMARY.md", "WORKFLOW_GUIDE.md"]
        for meta_file in meta_files:
            meta_path = self.devlog_dir / meta_file
            if meta_path.exists():
                result["meta"].append(meta_path)

        return result

    def scan_planning_files(self) -> Dict[str, List[Path]]:
        """Planning 폴더의 파일들을 카테고리별로 스캔"""
        result = {
            "system": [],
            "features": [],
            "ui": [],
            "reports": [],
            "presentations": [],
            "other": []
        }

        if not self.planning_dir.exists():
            return result

        # 카테고리 키워드 매칭
        keywords = {
            "system": ["System", "API", "Multiplayer", "Integration", "api_", "server_"],
            "features": ["Voice", "Loading", "proto", "Plan"],
            "ui": ["Figma", "UI", "UX", "Design"],
            "reports": ["Report", "Evaluation", "Assessment", "진행보고서"],
            "presentations": ["Presentation", "Weekly_Presentation"]
        }

        for md_file in sorted(self.planning_dir.glob("**/*.md")):
            # 하위 폴더 제외 여부 체크
            relative_path = md_file.relative_to(self.planning_dir)

            categorized = False
            file_name = md_file.name

            # 키워드로 카테고리 분류
            for category, kw_list in keywords.items():
                if any(kw.lower() in file_name.lower() for kw in kw_list):
                    result[category].append(md_file)
                    categorized = True
                    break

            if not categorized:
                result["other"].append(md_file)

        return result

    def get_relative_path(self, file_path: Path) -> str:
        """Documents 기준 상대 경로 반환"""
        try:
            rel_path = file_path.relative_to(self.base_dir)
            return rel_path.as_posix()
        except ValueError:
            return file_path.as_posix()

    def get_title_from_file(self, file_path: Path) -> str:
        """MD 파일에서 제목 추출"""
        try:
            with open(file_path, 'r', encoding='utf-8') as f:
                first_line = f.readline().strip()
                if first_line.startswith('#'):
                    return first_line.lstrip('#').strip()
        except:
            pass

        # 파일명에서 제목 생성
        return file_path.stem

    def format_agent_log_title(self, file_path: Path) -> str:
        """AgentLog 파일명을 읽기 좋은 제목으로 변환"""
        user = file_path.parent.name
        date_str = file_path.stem  # 예: 251108

        try:
            # YYMMDD 형식을 YYYY-MM-DD로 변환
            if len(date_str) == 6 and date_str.isdigit():
                year = "20" + date_str[:2]
                month = date_str[2:4]
                day = date_str[4:6]
                formatted_date = f"{year}-{month}-{day}"
                return f"{user} - {formatted_date}"
        except:
            pass

        return f"{user} - {file_path.stem}"

    def format_daily_log_title(self, file_path: Path) -> str:
        """Daily log 파일명 포맷팅"""
        return file_path.stem  # 2025-10-01 형식 그대로 사용

    def group_daily_logs_by_month(self, daily_files: List[Path]) -> Dict[str, List[Path]]:
        """Daily logs를 월별로 그룹화"""
        grouped = {}
        for file in daily_files:
            # 파일명에서 YYYY-MM 추출
            match = re.match(r'(\d{4}-\d{2})', file.stem)
            if match:
                month_key = match.group(1)
                if month_key not in grouped:
                    grouped[month_key] = []
                grouped[month_key].append(file)
        return grouped

    def generate_summary(self) -> str:
        """SUMMARY.md 내용 생성"""
        lines = ["# Summary", "", "## Introduction", "* [시작하기](README.md)", ""]

        # DevLog 섹션
        lines.append("## DevLog")
        lines.append("")

        devlog_files = self.scan_devlog_files()

        # Agent Logs
        if devlog_files["agent"]:
            lines.append("### Agent Logs")
            for file in devlog_files["agent"]:
                title = self.format_agent_log_title(file)
                rel_path = self.get_relative_path(file)
                lines.append(f"* [{title}]({rel_path})")
            lines.append("")

        # Daily Logs (월별 그룹화)
        if devlog_files["daily"]:
            grouped_daily = self.group_daily_logs_by_month(devlog_files["daily"])
            for month in sorted(grouped_daily.keys(), reverse=True):
                year, month_num = month.split('-')
                lines.append(f"### Daily Logs ({year}-{month_num})")
                for file in sorted(grouped_daily[month], reverse=True):
                    title = self.format_daily_log_title(file)
                    rel_path = self.get_relative_path(file)
                    lines.append(f"* [{title}]({rel_path})")
                lines.append("")

        # Weekly Summary
        if devlog_files["weekly"]:
            lines.append("### Weekly Summary")
            for file in devlog_files["weekly"]:
                title = self.get_title_from_file(file)
                rel_path = self.get_relative_path(file)
                lines.append(f"* [{title}]({rel_path})")
            lines.append("")

        # Monthly Summary
        if devlog_files["monthly"]:
            lines.append("### Monthly Summary")
            for file in devlog_files["monthly"]:
                title = self.get_title_from_file(file)
                rel_path = self.get_relative_path(file)
                lines.append(f"* [{title}]({rel_path})")
            lines.append("")

        # Meta files
        if devlog_files["meta"]:
            lines.append("### Documentation")
            for file in devlog_files["meta"]:
                title = self.get_title_from_file(file)
                rel_path = self.get_relative_path(file)
                lines.append(f"* [{title}]({rel_path})")
            lines.append("")

        # Planning 섹션
        lines.append("## Planning")
        lines.append("")

        planning_files = self.scan_planning_files()

        category_titles = {
            "system": "System Design",
            "features": "Feature Plans",
            "ui": "UI/UX Design",
            "reports": "Reports",
            "presentations": "Presentations",
            "other": "Other Documents"
        }

        for category, title in category_titles.items():
            if planning_files[category]:
                lines.append(f"### {title}")
                for file in sorted(planning_files[category]):
                    doc_title = self.get_title_from_file(file)
                    rel_path = self.get_relative_path(file)
                    lines.append(f"* [{doc_title}]({rel_path})")
                lines.append("")

        # External Links
        lines.append("## External Links")
        lines.append("* [GitHub Repository](https://github.com/doppleddiggong/YiSan)")
        lines.append("* [Doxygen Documentation](https://doppleddiggong.github.io/YiSan/doxygen/)")
        lines.append("")

        return "\n".join(lines)

    def write_summary(self):
        """SUMMARY.md 파일 작성"""
        summary_path = self.honkit_dir / "SUMMARY.md"
        content = self.generate_summary()

        with open(summary_path, 'w', encoding='utf-8') as f:
            f.write(content)

        print(f"✅ SUMMARY.md generated successfully at {summary_path}")
        print(f"📊 Total sections created")

        # 레거시 호환: Documents/HonkitPage/SUMMARY.md 동기화
        legacy_dir = self.base_dir / "HonkitPage"
        legacy_summary_path = legacy_dir / "SUMMARY.md"
        if legacy_summary_path.exists() or legacy_dir.exists():
            legacy_dir.mkdir(parents=True, exist_ok=True)
            with open(legacy_summary_path, 'w', encoding='utf-8') as legacy_file:
                legacy_file.write(content)
            print(f"🔁 Legacy SUMMARY.md synchronized at {legacy_summary_path}")

def main():
    generator = SummaryGenerator()
    generator.write_summary()

if __name__ == "__main__":
    main()
