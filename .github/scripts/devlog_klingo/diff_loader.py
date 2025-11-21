#!/usr/bin/env python3
"""
Git Diff 로더
커밋 타임스탬프 기준으로 Git diff와 커밋 메시지를 로딩
"""

import subprocess
from pathlib import Path


def run_git_command(cmd, cwd=None):
    """
    Git 명령 실행

    Args:
        cmd: 실행할 Git 명령 (리스트)
        cwd: 작업 디렉토리

    Returns:
        str: 명령 실행 결과
    """
    try:
        result = subprocess.run(
            cmd,
            cwd=cwd,
            capture_output=True,
            text=True,
            encoding='utf-8',
            errors='ignore'
        )
        return result.stdout.strip()
    except Exception as e:
        print(f"⚠️  Git 명령 실행 실패: {e}")
        return ""


def get_commits_in_range(since, until, repo_path=None):
    """
    특정 시간 범위의 커밋 해시 목록 가져오기

    Args:
        since: 시작 시간 (ISO 8601 형식)
        until: 종료 시간 (ISO 8601 형식)
        repo_path: Git 저장소 경로

    Returns:
        list: 커밋 해시 목록
    """
    cmd = [
        'git', 'log',
        f'--since={since}',
        f'--until={until}',
        '--pretty=format:%H'
    ]

    result = run_git_command(cmd, cwd=repo_path)

    if not result:
        return []

    return result.splitlines()


def get_commit_info(commit_hash, repo_path=None):
    """
    커밋 정보 가져오기

    Args:
        commit_hash: 커밋 해시
        repo_path: Git 저장소 경로

    Returns:
        dict: {
            "hash": 전체 해시,
            "short_hash": 짧은 해시,
            "author": 작성자,
            "date": 커밋 날짜,
            "subject": 제목,
            "body": 본문
        }
    """
    # 커밋 메타데이터 가져오기
    cmd = [
        'git', 'show',
        '--format=%H%n%h%n%an%n%ai%n%s%n%b',
        '--no-patch',
        commit_hash
    ]

    result = run_git_command(cmd, cwd=repo_path)

    if not result:
        return None

    lines = result.split('\n')

    # 최소 5줄은 있어야 함
    if len(lines) < 5:
        return None

    return {
        'hash': lines[0],
        'short_hash': lines[1],
        'author': lines[2],
        'date': lines[3],
        'subject': lines[4],
        'body': '\n'.join(lines[5:]).strip() if len(lines) > 5 else ''
    }


def get_diff_in_range(since, until, repo_path=None):
    """
    특정 시간 범위의 Git diff 가져오기

    Args:
        since: 시작 시간 (ISO 8601 형식)
        until: 종료 시간 (ISO 8601 형식)
        repo_path: Git 저장소 경로

    Returns:
        str: Git diff 결과
    """
    # 해당 범위의 커밋 목록 가져오기
    commits = get_commits_in_range(since, until, repo_path)

    if not commits:
        return ""

    # 첫 커밋의 부모 ~ 마지막 커밋의 diff
    first_commit = commits[-1]  # 가장 오래된 커밋
    last_commit = commits[0]    # 가장 최근 커밋

    # 첫 커밋의 부모 찾기
    cmd = ['git', 'rev-parse', f'{first_commit}^']
    parent = run_git_command(cmd, cwd=repo_path)

    if not parent:
        # 부모가 없으면 (첫 커밋) 해당 커밋만 diff
        cmd = ['git', 'show', first_commit]
    else:
        # 부모 ~ 마지막 커밋의 diff
        cmd = ['git', 'diff', f'{parent}..{last_commit}']

    return run_git_command(cmd, cwd=repo_path)


def get_changed_files_in_range(since, until, repo_path=None):
    """
    특정 시간 범위에서 변경된 파일 목록 가져오기

    Args:
        since: 시작 시간 (ISO 8601 형식)
        until: 종료 시간 (ISO 8601 형식)
        repo_path: Git 저장소 경로

    Returns:
        list: 변경된 파일 경로 목록
    """
    commits = get_commits_in_range(since, until, repo_path)

    if not commits:
        return []

    # 첫 커밋 ~ 마지막 커밋의 변경 파일
    first_commit = commits[-1]
    last_commit = commits[0]

    cmd = ['git', 'diff', '--name-only', f'{first_commit}^..{last_commit}']
    result = run_git_command(cmd, cwd=repo_path)

    if not result:
        return []

    return [f.strip() for f in result.splitlines() if f.strip()]


def get_commit_messages_in_range(since, until, repo_path=None):
    """
    특정 시간 범위의 모든 커밋 메시지 가져오기

    Args:
        since: 시작 시간 (ISO 8601 형식)
        until: 종료 시간 (ISO 8601 형식)
        repo_path: Git 저장소 경로

    Returns:
        list: 커밋 정보 딕셔너리 목록
    """
    commits = get_commits_in_range(since, until, repo_path)

    commit_messages = []
    for commit_hash in commits:
        info = get_commit_info(commit_hash, repo_path)
        if info:
            commit_messages.append(info)

    return commit_messages


if __name__ == "__main__":
    # 테스트 코드
    from datetime import datetime, timedelta, timezone
    import sys

    print("=== Git Diff 로더 테스트 ===")

    # 프로젝트 루트 경로
    script_dir = Path(__file__).parent
    project_root = script_dir.parent.parent.parent

    print(f"Git 저장소: {project_root}")

    # 어제 날짜 기준으로 테스트
    KST = timezone(timedelta(hours=9))
    now_kst = datetime.now(KST)
    yesterday = (now_kst.date() - timedelta(days=1))

    since_kst = datetime.combine(yesterday, datetime.min.time()).replace(tzinfo=KST)
    until_kst = datetime.combine(yesterday + timedelta(days=1), datetime.min.time()).replace(tzinfo=KST)

    since_str = since_kst.isoformat()
    until_str = until_kst.isoformat()

    print(f"\n날짜 범위:")
    print(f"  Since: {since_str}")
    print(f"  Until: {until_str}")

    # 커밋 목록
    commits = get_commits_in_range(since_str, until_str, project_root)
    print(f"\n커밋 개수: {len(commits)}")

    if commits:
        print(f"\n최근 커밋 정보:")
        info = get_commit_info(commits[0], project_root)
        if info:
            print(f"  해시: {info['short_hash']}")
            print(f"  작성자: {info['author']}")
            print(f"  날짜: {info['date']}")
            print(f"  제목: {info['subject']}")

        # 변경된 파일
        files = get_changed_files_in_range(since_str, until_str, project_root)
        print(f"\n변경된 파일: {len(files)}개")
        if files:
            for f in files[:5]:
                print(f"  - {f}")
            if len(files) > 5:
                print(f"  ... 외 {len(files) - 5}개")

        # 커밋 메시지
        messages = get_commit_messages_in_range(since_str, until_str, project_root)
        print(f"\n커밋 메시지: {len(messages)}개")
        if messages:
            for msg in messages[:3]:
                print(f"\n  [{msg['short_hash']}] {msg['subject']}")
                if msg['body']:
                    body_preview = msg['body'][:100].replace('\n', ' ')
                    print(f"    {body_preview}...")
    else:
        print("⚠️  해당 범위에 커밋이 없습니다")
