#!/usr/bin/env python3
"""
Weekly Metrics Aggregator
Daily DevLog 메트릭 파일을 집계하여 주간 메트릭을 생성합니다.
"""

import argparse
import json
import datetime
from pathlib import Path
from collections import defaultdict


def get_week_range(date_str=None):
    """주간 범위 계산 (월요일 ~ 일요일)"""
    if date_str:
        target = datetime.datetime.strptime(date_str, "%Y-%m-%d").date()
    else:
        target = datetime.date.today()

    # 월요일 찾기
    monday = target - datetime.timedelta(days=target.weekday())
    sunday = monday + datetime.timedelta(days=6)

    # ISO week number
    week_num = monday.isocalendar()[1]
    year = monday.year

    return {
        "monday": monday,
        "sunday": sunday,
        "week_label": f"{year}-W{week_num:02d}",
        "year": year,
        "week": week_num,
        "date_range": f"{monday.strftime('%Y-%m-%d')} ~ {sunday.strftime('%Y-%m-%d')}"
    }


def aggregate_weekly_metrics(devlog_dir, week_info):
    """주간 Daily 메트릭 집계"""
    devlog_path = Path(devlog_dir)

    # 집계 데이터 초기화
    total_commits = 0
    total_additions = 0
    total_deletions = 0
    total_files_changed = 0
    authors_set = set()
    commit_types = defaultdict(int)
    all_top_changes = []
    active_days = 0

    current = week_info["monday"]
    dates_processed = []

    while current <= week_info["sunday"]:
        date_str = current.strftime("%Y-%m-%d")
        metrics_file = devlog_path / f"{date_str}.metrics.json"

        if metrics_file.exists():
            try:
                with metrics_file.open(encoding="utf-8") as f:
                    daily_data = json.load(f)

                # 메트릭 집계
                total_commits += daily_data.get("commit_count", 0)
                total_additions += daily_data.get("additions", 0)
                total_deletions += daily_data.get("deletions", 0)
                total_files_changed += daily_data.get("files_changed", 0)

                # 저자 집합
                daily_authors = daily_data.get("authors", [])
                authors_set.update(daily_authors)

                # 커밋 타입 집계
                daily_types = daily_data.get("commit_types", {})
                for ctype, count in daily_types.items():
                    commit_types[ctype] += count

                # 주요 변경 사항 수집
                daily_top_changes = daily_data.get("top_changes", [])
                for change in daily_top_changes:
                    change_copy = change.copy()
                    change_copy["date"] = date_str
                    all_top_changes.append(change_copy)

                active_days += 1
                dates_processed.append(date_str)

            except Exception as e:
                print(f"⚠️ {date_str}.metrics.json 읽기 실패: {e}")

        current += datetime.timedelta(days=1)

    # 주요 변경 사항 정렬 (최대 10개, 커밋 타입 우선순위: feat > fix > refactor > docs > chore)
    type_priority = {"feat": 1, "fix": 2, "refactor": 3, "docs": 4, "chore": 5, "other": 6}
    all_top_changes.sort(key=lambda x: (
        type_priority.get(x.get("type", "other"), 99),
        x.get("date", "")
    ))
    top_10_changes = all_top_changes[:10]

    # 주간 메트릭 JSON 구조
    weekly_metrics = {
        "week_label": week_info["week_label"],
        "date_range": week_info["date_range"],
        "generated_at": datetime.datetime.now(datetime.timezone.utc).isoformat(),
        "period": {
            "start": week_info["monday"].strftime("%Y-%m-%d"),
            "end": week_info["sunday"].strftime("%Y-%m-%d")
        },
        "commit_count": total_commits,
        "additions": total_additions,
        "deletions": total_deletions,
        "files_changed": total_files_changed,
        "author_count": len(authors_set),
        "authors": sorted(list(authors_set)),
        "commit_types": dict(commit_types),
        "active_days": active_days,
        "total_days": 7,
        "dates_processed": dates_processed,
        "top_changes": top_10_changes
    }

    return weekly_metrics


def main():
    ap = argparse.ArgumentParser(description="Weekly Metrics Aggregator")
    ap.add_argument("--date", default=None, help="기준 날짜 (YYYY-MM-DD, 비워두면 이번 주)")
    ap.add_argument("--week-label", default=None, help="직접 주차 지정 (YYYY-W##)")
    ap.add_argument("--devlog-dir", required=True, help="Daily DevLog 디렉토리")
    ap.add_argument("--output", required=True, help="출력 메트릭 JSON 파일 경로")
    args = ap.parse_args()

    # 주간 범위 계산
    if args.week_label:
        # YYYY-W## 형식에서 날짜 계산
        year, week = args.week_label.split('-W')
        year = int(year)
        week = int(week)
        # ISO 주차에서 월요일 계산
        jan_4 = datetime.date(year, 1, 4)
        monday = jan_4 - datetime.timedelta(days=jan_4.weekday())
        monday = monday + datetime.timedelta(weeks=week - 1)
        week_info = get_week_range(monday.strftime("%Y-%m-%d"))
    else:
        week_info = get_week_range(args.date)

    print(f"📅 주차: {week_info['week_label']}")
    print(f"   기간: {week_info['date_range']}")

    # 메트릭 집계
    print(f"📊 Daily 메트릭 집계 중...")
    weekly_metrics = aggregate_weekly_metrics(args.devlog_dir, week_info)

    print(f"   활동 일수: {weekly_metrics['active_days']}/{weekly_metrics['total_days']}일")
    print(f"   총 커밋: {weekly_metrics['commit_count']}개")
    print(f"   변경량: +{weekly_metrics['additions']} / -{weekly_metrics['deletions']}")
    print(f"   참여 개발자: {weekly_metrics['author_count']}명")

    if weekly_metrics['commit_count'] == 0:
        print("⚠️ 해당 주차에 커밋이 없습니다.")

    # 메트릭 JSON 저장
    output_path = Path(args.output)
    output_path.parent.mkdir(parents=True, exist_ok=True)

    with output_path.open('w', encoding='utf-8') as f:
        json.dump(weekly_metrics, f, ensure_ascii=False, indent=2)

    print(f"✅ 주간 메트릭 생성 완료: {args.output}")

    return 0


if __name__ == "__main__":
    exit(main())
