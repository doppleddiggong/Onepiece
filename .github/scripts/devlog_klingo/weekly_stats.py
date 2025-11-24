#!/usr/bin/env python3
"""
주간 DevLog 통계 분석 모듈
Daily DevLog들을 수집하여 주간 통계 생성
"""

import json
from pathlib import Path
from datetime import date, timedelta
from collections import Counter


def get_week_range(target_date):
    """
    주어진 날짜가 속한 주의 시작일(월요일)과 종료일(일요일) 반환

    Args:
        target_date: 대상 날짜 (date 객체)

    Returns:
        tuple: (시작일, 종료일)
    """
    # 해당 주의 월요일 찾기
    weekday = target_date.weekday()  # 0=월요일, 6=일요일
    start_date = target_date - timedelta(days=weekday)
    end_date = start_date + timedelta(days=6)

    return start_date, end_date


def collect_daily_devlogs(daily_dir, start_date, end_date):
    """
    주간 범위의 Daily DevLog 파일들을 수집

    Args:
        daily_dir: Daily DevLog 디렉토리 경로
        start_date: 시작 날짜
        end_date: 종료 날짜

    Returns:
        list: DevLog 파일 경로 리스트
    """
    daily_path = Path(daily_dir)
    if not daily_path.exists():
        print(f"[WARN] Daily DevLog 디렉토리를 찾을 수 없습니다: {daily_dir}")
        return []

    devlogs = []
    current = start_date

    while current <= end_date:
        # YYYY-MM-DD.md 형식의 파일 찾기
        devlog_file = daily_path / f"{current.isoformat()}.md"

        if devlog_file.exists():
            devlogs.append(devlog_file)

        current += timedelta(days=1)

    return devlogs


def collect_metrics(daily_dir, start_date, end_date, branch_filter=None):
    """
    주간 범위의 메트릭 JSON 파일들을 수집

    Args:
        daily_dir: Daily DevLog 디렉토리 경로
        start_date: 시작 날짜
        end_date: 종료 날짜
        branch_filter: 필터링할 브랜치명 (None이면 모두 수집)

    Returns:
        list: 메트릭 데이터 리스트
    """
    daily_path = Path(daily_dir)
    if not daily_path.exists():
        return []

    metrics_list = []
    current = start_date

    while current <= end_date:
        # YYYY-MM-DD.metrics.json 형식의 파일 찾기
        metrics_file = daily_path / f"{current.isoformat()}.metrics.json"

        if metrics_file.exists():
            try:
                with open(metrics_file, 'r', encoding='utf-8') as f:
                    metrics = json.load(f)

                    # 브랜치 필터링
                    if branch_filter:
                        metrics_branch = metrics.get('branch', '')
                        if metrics_branch != branch_filter:
                            continue  # 다른 브랜치면 건너뛰기

                    metrics_list.append(metrics)
            except Exception as e:
                print(f"[WARN] 메트릭 파일 로딩 실패 ({metrics_file}): {e}")

        current += timedelta(days=1)

    return metrics_list


def calculate_weekly_stats(metrics_list):
    """
    주간 통계 계산

    Args:
        metrics_list: 메트릭 데이터 리스트

    Returns:
        dict: 주간 통계
    """
    if not metrics_list:
        return {
            'total_days': 0,
            'total_commits': 0,
            'total_files_changed': 0,
            'developers': [],
            'top_systems': []
        }

    # 기본 통계
    total_commits = sum(m.get('commit_count', 0) for m in metrics_list)
    total_files = sum(m.get('files_changed', 0) for m in metrics_list)

    # 개발자 목록 (중복 제거)
    developers = list(set(m.get('developer', 'Unknown') for m in metrics_list))

    # 시스템별 통계 집계
    system_counter = Counter()
    for metrics in metrics_list:
        top_systems = metrics.get('top_systems', [])
        for system, count in top_systems:
            system_counter[system] += count

    # 상위 시스템 추출
    top_systems = system_counter.most_common(10)

    stats = {
        'total_days': len(metrics_list),
        'total_commits': total_commits,
        'total_files_changed': total_files,
        'avg_commits_per_day': round(total_commits / len(metrics_list), 1) if metrics_list else 0,
        'avg_files_per_day': round(total_files / len(metrics_list), 1) if metrics_list else 0,
        'developers': developers,
        'developer_count': len(developers),
        'top_systems': top_systems
    }

    return stats


def read_daily_devlog_content(devlog_path):
    """
    Daily DevLog 파일의 본문을 읽어옴

    Args:
        devlog_path: DevLog 파일 경로

    Returns:
        str: DevLog 본문
    """
    try:
        with open(devlog_path, 'r', encoding='utf-8') as f:
            return f.read()
    except Exception as e:
        print(f"[WARN] DevLog 파일 읽기 실패 ({devlog_path}): {e}")
        return ""


def extract_daily_summaries(devlogs):
    """
    Daily DevLog들에서 주요 변경 사항 섹션을 추출

    Args:
        devlogs: DevLog 파일 경로 리스트

    Returns:
        list: 날짜별 주요 변경 사항 리스트
    """
    summaries = []

    for devlog_path in devlogs:
        content = read_daily_devlog_content(devlog_path)

        if not content:
            continue

        # 날짜 추출 (파일명에서)
        date_str = devlog_path.stem  # YYYY-MM-DD

        # "## 1. 오늘의 주요 변경" 섹션 추출
        lines = content.split('\n')
        in_section = False
        section_content = []

        for line in lines:
            if line.startswith('## 1. 오늘의 주요 변경') or line.startswith('## 1. 주요 변경'):
                in_section = True
                continue

            if in_section and line.startswith('## '):
                # 다음 섹션 시작
                break

            if in_section and line.strip():
                section_content.append(line.strip())

        if section_content:
            summaries.append({
                'date': date_str,
                'summary': '\n'.join(section_content)
            })

    return summaries


def generate_weekly_summary_data(daily_dir, start_date, end_date, branch_filter='main'):
    """
    주간 DevLog 생성에 필요한 모든 데이터 수집 및 통계 계산

    Args:
        daily_dir: Daily DevLog 디렉토리 경로
        start_date: 시작 날짜
        end_date: 종료 날짜
        branch_filter: 필터링할 브랜치명 (기본: main)

    Returns:
        dict: 주간 요약 데이터
    """
    print(f"\n=== 주간 데이터 수집 ===")
    print(f"기간: {start_date} ~ {end_date}")
    print(f"브랜치: {branch_filter}")

    # Daily DevLog 수집
    devlogs = collect_daily_devlogs(daily_dir, start_date, end_date)
    print(f"[1/3] Daily DevLog: {len(devlogs)}개 수집")

    # 메트릭 수집 (브랜치 필터링)
    metrics_list = collect_metrics(daily_dir, start_date, end_date, branch_filter)
    print(f"[2/3] 메트릭 파일: {len(metrics_list)}개 수집 (브랜치: {branch_filter})")

    # 통계 계산
    stats = calculate_weekly_stats(metrics_list)
    print(f"[3/3] 주간 통계 계산 완료")

    # 일별 요약 추출
    daily_summaries = extract_daily_summaries(devlogs)

    data = {
        'start_date': start_date.isoformat(),
        'end_date': end_date.isoformat(),
        'week_number': start_date.isocalendar()[1],
        'year': start_date.year,
        'branch': branch_filter,
        'stats': stats,
        'daily_summaries': daily_summaries,
        'devlog_files': [str(d) for d in devlogs]
    }

    return data


if __name__ == "__main__":
    """
    테스트 실행
    """
    from datetime import date

    # 이번 주 범위 계산
    today = date.today()
    start, end = get_week_range(today)

    print(f"이번 주: {start} ~ {end}")

    # 프로젝트 루트 기준 경로
    project_root = Path(__file__).parent.parent.parent.parent
    daily_dir = project_root / 'Documents' / 'DevLog' / 'Daily'

    # 데이터 수집 (기본: main 브랜치)
    data = generate_weekly_summary_data(daily_dir, start, end, 'main')

    # 결과 출력
    print("\n=== 주간 통계 ===")
    print(json.dumps(data['stats'], ensure_ascii=False, indent=2))
