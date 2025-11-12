#!/usr/bin/env python3
"""
SUMMARY.md 자동 생성기
Documents 폴더를 스캔하여 HonKit용 SUMMARY.md를 자동으로 생성합니다.
"""

import argparse
import re
from pathlib import Path
from collections import defaultdict
from datetime import datetime

def scan_devlog(devlog_dir):
    """DevLog 폴더 스캔 (Daily, Weekly, Monthly 하위 폴더 지원)"""
    devlog_path = Path(devlog_dir)
    if not devlog_path.exists():
        return {}

    # 날짜별로 그룹화
    daily_logs = defaultdict(list)
    weekly_logs = []
    monthly_logs = []

    # Daily 폴더 스캔
    daily_dir = devlog_path / "Daily"
    if daily_dir.exists():
        for md_file in sorted(daily_dir.glob("*.md"), reverse=True):
            filename = md_file.stem
            # 일일 로그 (YYYY-MM-DD 형식)
            if re.match(r"\d{4}-\d{2}-\d{2}", filename):
                year_month = filename[:7]  # YYYY-MM
                daily_logs[year_month].append((filename, f"Daily/{md_file.name}"))

    # Weekly 폴더 스캔
    weekly_dir = devlog_path / "Weekly"
    if weekly_dir.exists():
        for md_file in sorted(weekly_dir.glob("*.md"), reverse=True):
            filename = md_file.stem
            if "W" in filename and "Summary" in filename:
                weekly_logs.append((filename, f"Weekly/{md_file.name}"))

    # Monthly 폴더 스캔
    monthly_dir = devlog_path / "Monthly"
    if monthly_dir.exists():
        for md_file in sorted(monthly_dir.glob("*.md"), reverse=True):
            filename = md_file.stem
            monthly_logs.append((filename, f"Monthly/{md_file.name}"))

    # 루트의 특수 Summary 파일들 (_Last30Summary.md 등)
    for md_file in sorted(devlog_path.glob("*.md"), reverse=True):
        filename = md_file.stem
        if "Summary" in filename:
            weekly_logs.append((filename, md_file.name))

    return {
        "daily": daily_logs,
        "weekly": weekly_logs,
        "monthly": monthly_logs
    }

def scan_planning(planning_dir):
    """Planning 폴더 스캔"""
    planning_path = Path(planning_dir)
    if not planning_path.exists():
        return {}

    categories = {
        "system_design": [],
        "feature_plans": [],
        "ui_design": [],
        "reports": [],
        "presentations": []
    }

    for md_file in sorted(planning_path.glob("*.md")):
        filename = md_file.stem
        name = md_file.name

        # 카테고리별 분류
        if any(keyword in filename.lower() for keyword in ["multiplayer", "integration", "api", "server"]):
            categories["system_design"].append((filename, name))
        elif any(keyword in filename.lower() for keyword in ["figma", "ui", "design"]):
            categories["ui_design"].append((filename, name))
        elif any(keyword in filename.lower() for keyword in ["report", "evaluation", "assessment", "진행보고서"]):
            categories["reports"].append((filename, name))
        elif any(keyword in filename.lower() for keyword in ["presentation", "발표"]):
            categories["presentations"].append((filename, name))
        else:
            categories["feature_plans"].append((filename, name))

    return categories

def format_title(filename):
    """파일명을 제목으로 변환"""
    # 날짜 형식 (YYYY-MM-DD)
    if re.match(r"\d{4}-\d{2}-\d{2}", filename):
        return filename

    # 언더스코어를 공백으로
    title = filename.replace("_", " ")

    # 앞의 날짜 제거 (YYYY-MM-DD- 형식)
    title = re.sub(r"^\d{4}-\d{2}-\d{2}-", "", title)

    return title

def scan_meetings(meeting_dir):
    """Meeting 폴더 스캔"""
    meeting_path = Path(meeting_dir)
    if not meeting_path.exists():
        return []

    meeting_logs = []
    for md_file in sorted(meeting_path.glob("*.md"), reverse=True):
        filename = md_file.stem
        # 'Meeting_' 또는 '회의록_' 접두사를 제거하고 제목으로 사용
        title = re.sub(r'^(Meeting_|회의록_)', '', filename)
        meeting_logs.append((title, f"Meeting/{md_file.name}"))
    
    return meeting_logs

def generate_summary(docs_root):
    """SUMMARY.md 생성"""
    devlog_dir = Path(docs_root) / "DevLog"
    planning_dir = Path(docs_root) / "Planning"
    meeting_dir = Path(docs_root) / "Meeting"

    devlog_data = scan_devlog(devlog_dir)
    planning_data = scan_planning(planning_dir)
    meeting_data = scan_meetings(meeting_dir)

    lines = ["# Summary\n"]

    # Introduction
    lines.append("## Introduction")
    lines.append("* [시작하기](README.md)\n")

    # DevLog 섹션
    lines.append("## DevLog\n")

    # 일일 로그 (월별 그룹화)
    if devlog_data["daily"]:
        for year_month in sorted(devlog_data["daily"].keys(), reverse=True):
            # YYYY-MM을 표시 형식으로 변환
            year, month = year_month.split("-")
            lines.append(f"### Daily Logs ({year}-{month})")

            for filename, name in sorted(devlog_data["daily"][year_month], reverse=True):
                lines.append(f"* [{filename}](DevLog/{name})")

            lines.append("")

    # 주간 요약
    if devlog_data["weekly"]:
        lines.append("### Weekly Summary")
        for filename, name in sorted(devlog_data["weekly"], reverse=True):
            title = format_title(filename)
            lines.append(f"* [{title}](DevLog/{name})")
        lines.append("")

    # 월간 요약
    if devlog_data["monthly"]:
        lines.append("### Monthly Summary")
        for filename, name in sorted(devlog_data["monthly"], reverse=True):
            title = format_title(filename)
            lines.append(f"* [{title}](DevLog/{name})")
        lines.append("")

    # 회의록 섹션 추가
    if meeting_data:
        lines.append("## 회의록\n")
        for title, path in meeting_data:
            lines.append(f"* [{title}]({path})")
        lines.append("")


    # Planning 섹션
    lines.append("## Planning\n")

    # System Design
    if planning_data["system_design"]:
        lines.append("### System Design")
        for filename, name in sorted(planning_data["system_design"]):
            title = format_title(filename)
            lines.append(f"* [{title}](Planning/{name})")
        lines.append("")

    # Feature Plans
    if planning_data["feature_plans"]:
        lines.append("### Feature Plans")
        for filename, name in sorted(planning_data["feature_plans"]):
            title = format_title(filename)
            lines.append(f"* [{title}](Planning/{name})")
        lines.append("")

    # UI/UX Design
    if planning_data["ui_design"]:
        lines.append("### UI/UX Design")
        for filename, name in sorted(planning_data["ui_design"]):
            title = format_title(filename)
            lines.append(f"* [{title}](Planning/{name})")
        lines.append("")

    # Reports
    if planning_data["reports"]:
        lines.append("### Reports")
        for filename, name in sorted(planning_data["reports"], reverse=True):
            title = format_title(filename)
            lines.append(f"* [{title}](Planning/{name})")
        lines.append("")

    # Presentations
    if planning_data["presentations"]:
        lines.append("### Presentations")
        for filename, name in sorted(planning_data["presentations"]):
            title = format_title(filename)
            lines.append(f"* [{title}](Planning/{name})")
        lines.append("")

    # External Links
    lines.append("## External Links")
    lines.append("* [GitHub Repository](https://github.com/doppleddiggong/YiSan)")
    lines.append("* [Doxygen Documentation](https://doppleddiggong.github.io/YiSan/doxygen/)")

    return "\n".join(lines)

def main():
    ap = argparse.ArgumentParser(description="SUMMARY.md Generator for HonKit")
    ap.add_argument("docs_root", help="Documents 루트 디렉토리")
    ap.add_argument("--out", default=None, help="출력 파일 경로 (기본: docs_root/SUMMARY.md)")
    args = ap.parse_args()

    # SUMMARY.md 생성
    summary_content = generate_summary(args.docs_root)

    # 출력
    if args.out:
        out_path = Path(args.out)
    else:
        out_path = Path(args.docs_root) / "SUMMARY.md"

    out_path.write_text(summary_content, encoding="utf-8")
    print(f"✅ SUMMARY.md generated: {out_path}")

if __name__ == "__main__":
    main()
