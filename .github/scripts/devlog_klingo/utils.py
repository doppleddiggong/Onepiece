#!/usr/bin/env python3
"""
유틸리티 함수 모음
날짜 계산, 파일 로딩 등의 공통 기능 제공
"""

from datetime import datetime, timedelta, timezone
from pathlib import Path
import csv


# KST 타임존 정의
KST = timezone(timedelta(hours=9))


def get_devlog_date(now_utc=None):
    """
    DevLog 생성 날짜를 KST 기준으로 계산

    UTC 00:00 = KST 09:00이므로,
    DevLog 생성 시점이 UTC 2025-11-21 00:00이면
    참조해야 할 날짜는 2025-11-20이다.

    Args:
        now_utc: UTC 기준 현재 시간 (datetime 객체). None이면 현재 시간 사용.

    Returns:
        date 객체 (KST 기준 어제 날짜)
    """
    if now_utc is None:
        now_utc = datetime.now(timezone.utc)

    # UTC를 KST로 변환
    now_kst = now_utc.astimezone(KST)

    # 어제 날짜 (KST 기준)
    devlog_date = now_kst.date() - timedelta(days=1)

    return devlog_date


def get_git_date_range(target_date):
    """
    특정 날짜의 Git 커밋 범위를 계산

    Args:
        target_date: date 객체 (KST 기준)

    Returns:
        tuple: (since, until) - Git log에서 사용할 날짜 범위 문자열
    """
    # 전날 00:00 ~ 당일 00:00 (KST)
    since_kst = datetime.combine(target_date, datetime.min.time()).replace(tzinfo=KST)
    until_kst = datetime.combine(target_date + timedelta(days=1), datetime.min.time()).replace(tzinfo=KST)

    # ISO 8601 형식으로 변환
    since_str = since_kst.isoformat()
    until_str = until_kst.isoformat()

    return since_str, until_str


def load_developers(csv_path):
    """
    개발자 정보 CSV 로딩

    Args:
        csv_path: developers.csv 파일 경로

    Returns:
        dict: {branch: {"developer": name, "jira_account": account}}
    """
    developers = {}

    csv_file = Path(csv_path)
    if not csv_file.exists():
        print(f"[WARN]  developers.csv not found: {csv_path}")
        return developers

    with open(csv_file, 'r', encoding='utf-8') as f:
        reader = csv.DictReader(f)
        for row in reader:
            branch = row['branch'].strip()
            developers[branch] = {
                'developer': row['developer'].strip(),
                'jira_account': row['jira_account'].strip()
            }

    return developers


def get_developer_from_branch(branch_name, developers):
    """
    브랜치명에서 개발자 정보 추출

    Args:
        branch_name: Git 브랜치명
        developers: load_developers() 결과

    Returns:
        dict: {"developer": name, "jira_account": account} 또는 None
    """
    return developers.get(branch_name)


def format_date_korean(date_obj):
    """
    날짜를 한국어 형식으로 포맷팅

    Args:
        date_obj: date 객체

    Returns:
        str: "YYYY년 MM월 DD일 (요일)" 형식
    """
    weekdays = ['월', '화', '수', '목', '금', '토', '일']
    weekday = weekdays[date_obj.weekday()]

    return f"{date_obj.year}년 {date_obj.month:02d}월 {date_obj.day:02d}일 ({weekday})"


def ensure_dir(path):
    """
    디렉토리가 존재하지 않으면 생성

    Args:
        path: 디렉토리 경로 (str 또는 Path)
    """
    Path(path).mkdir(parents=True, exist_ok=True)


if __name__ == "__main__":
    # 테스트 코드
    print("=== 날짜 계산 테스트 ===")

    # 현재 시간 기준 DevLog 날짜
    devlog_date = get_devlog_date()
    print(f"DevLog 날짜 (KST 기준 어제): {devlog_date}")
    print(f"한국어 형식: {format_date_korean(devlog_date)}")

    # Git 날짜 범위
    since, until = get_git_date_range(devlog_date)
    print(f"\nGit 날짜 범위:")
    print(f"  Since: {since}")
    print(f"  Until: {until}")

    # 개발자 정보 로딩 테스트
    print("\n=== 개발자 정보 로딩 테스트 ===")
    developers_csv = Path(__file__).parent.parent.parent / "data" / "developers.csv"
    devs = load_developers(developers_csv)
    print(f"로딩된 개발자: {len(devs)}명")
    for branch, info in devs.items():
        print(f"  {branch}: {info['developer']} ({info['jira_account']})")
