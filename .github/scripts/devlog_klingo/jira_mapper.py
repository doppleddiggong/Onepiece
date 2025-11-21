#!/usr/bin/env python3
"""
Jira 매핑 시스템
파일 경로 → 시스템 매핑 및 Jira API 연동
"""

import yaml
import requests
from pathlib import Path
from collections import Counter


def load_jira_rules(rules_path):
    """
    Jira 매핑 룰 YAML 로딩

    Args:
        rules_path: jira_map_rules.yaml 경로

    Returns:
        dict: {시스템명: [경로 prefix 리스트]}
    """
    rules_file = Path(rules_path)

    if not rules_file.exists():
        print(f"[WARN]  Jira 매핑 룰 파일을 찾을 수 없습니다: {rules_path}")
        return {}

    with open(rules_file, 'r', encoding='utf-8') as f:
        rules = yaml.safe_load(f)

    return rules if rules else {}


def map_files_to_systems(changed_files, jira_rules):
    """
    변경된 파일 목록을 시스템명으로 매핑

    Args:
        changed_files: 변경된 파일 경로 리스트
        jira_rules: load_jira_rules() 결과

    Returns:
        Counter: {시스템명: 변경 파일 수}
    """
    system_counter = Counter()

    for file_path in changed_files:
        # 파일 경로를 정규화 (백슬래시 → 슬래시)
        normalized_path = file_path.replace('\\', '/')

        # 각 시스템 룰과 비교
        for system_name, path_prefixes in jira_rules.items():
            for prefix in path_prefixes:
                # prefix를 정규화
                normalized_prefix = prefix.replace('\\', '/')

                if normalized_path.startswith(normalized_prefix):
                    system_counter[system_name] += 1
                    break  # 한 시스템에 매핑되면 다음 파일로

    return system_counter


def get_top_systems(system_counter, max_count=3):
    """
    가장 많이 변경된 상위 시스템 반환

    Args:
        system_counter: map_files_to_systems() 결과
        max_count: 반환할 최대 개수

    Returns:
        list: [(시스템명, 변경 파일 수), ...]
    """
    return system_counter.most_common(max_count)


def fetch_jira_issues(jira_url, jira_email, jira_token, jira_account, jira_project=None, max_results=10):
    """
    Jira API를 통해 특정 작업자의 최근 이슈 조회

    Args:
        jira_url: Jira 서버 URL (예: https://your-domain.atlassian.net)
        jira_email: Jira 계정 이메일
        jira_token: Jira API 토큰
        jira_account: 작업자 Jira 계정명
        jira_project: 프로젝트 키 (선택, 예: UN)
        max_results: 최대 결과 개수

    Returns:
        list: 이슈 정보 딕셔너리 리스트
    """
    try:
        # Jira REST API 엔드포인트
        api_url = f"{jira_url}/rest/api/3/search"

        # JQL 쿼리: 특정 작업자에게 할당된 이슈
        if jira_project:
            jql = f'project = "{jira_project}" AND assignee = "{jira_account}" ORDER BY updated DESC'
        else:
            jql = f'assignee = "{jira_account}" ORDER BY updated DESC'

        params = {
            'jql': jql,
            'maxResults': max_results,
            'fields': 'summary,status,priority,issuetype,updated'
        }

        # 인증 (Basic Auth)
        auth = (jira_email, jira_token)

        response = requests.get(api_url, params=params, auth=auth, timeout=10)
        response.raise_for_status()

        data = response.json()

        issues = []
        for issue in data.get('issues', []):
            issues.append({
                'key': issue['key'],
                'summary': issue['fields']['summary'],
                'status': issue['fields']['status']['name'],
                'priority': issue['fields']['priority']['name'] if issue['fields'].get('priority') else 'None',
                'type': issue['fields']['issuetype']['name'],
                'updated': issue['fields']['updated']
            })

        return issues

    except requests.exceptions.RequestException as e:
        print(f"[WARN]  Jira API 호출 실패: {e}")
        return []

    except Exception as e:
        print(f"[WARN]  Jira 이슈 파싱 실패: {e}")
        return []


def format_jira_issues(issues):
    """
    Jira 이슈를 텍스트 형식으로 포맷팅

    Args:
        issues: fetch_jira_issues() 결과

    Returns:
        str: 포맷팅된 이슈 목록
    """
    if not issues:
        return "조회된 Jira 이슈가 없습니다."

    lines = []
    for issue in issues:
        lines.append(f"- [{issue['key']}] {issue['summary']}")
        lines.append(f"  상태: {issue['status']}, 우선순위: {issue['priority']}, 타입: {issue['type']}")

    return '\n'.join(lines)


if __name__ == "__main__":
    # 테스트 코드
    print("=== Jira 매퍼 테스트 ===")

    # 프로젝트 루트 경로
    script_dir = Path(__file__).parent
    project_root = script_dir.parent.parent.parent

    # Jira 룰 로딩
    rules_path = project_root / ".github" / "data" / "jira_map_rules.yaml"
    jira_rules = load_jira_rules(rules_path)

    print(f"\nJira 룰 로딩: {len(jira_rules)}개 시스템")
    for system, prefixes in jira_rules.items():
        print(f"  {system}: {len(prefixes)}개 경로 패턴")

    # 테스트 파일 목록
    test_files = [
        "Source/Onepiece/Network/Private/NetworkData.cpp",
        "Source/Onepiece/Network/Public/NetworkData.h",
        "Source/Onepiece/Voice/Private/UVoiceConversationSystem.cpp",
        "Documents/DevLog/AgentLog/test.md"
    ]

    print(f"\n테스트 파일: {len(test_files)}개")

    # 파일 → 시스템 매핑
    system_counter = map_files_to_systems(test_files, jira_rules)

    print(f"\n매핑 결과:")
    for system, count in system_counter.most_common():
        print(f"  {system}: {count}개 파일")

    # 상위 시스템
    top_systems = get_top_systems(system_counter, max_count=3)
    print(f"\n상위 시스템 (최대 3개):")
    for system, count in top_systems:
        print(f"  {system}: {count}개 파일")

    # Jira API 테스트 (실제 API 호출은 주석 처리)
    print("\n=== Jira API 테스트 ===")
    print("[WARN]  Jira API 호출은 환경 변수 설정 후 사용하세요")
    print("필요한 환경 변수:")
    print("  - JIRA_URL: Jira 서버 URL")
    print("  - JIRA_EMAIL: Jira 계정 이메일")
    print("  - JIRA_TOKEN: Jira API 토큰")

    # 예시:
    # import os
    # jira_url = os.getenv('JIRA_URL')
    # jira_email = os.getenv('JIRA_EMAIL')
    # jira_token = os.getenv('JIRA_TOKEN')
    # jira_account = "jubaeck Bae"
    #
    # issues = fetch_jira_issues(jira_url, jira_email, jira_token, jira_account)
    # print(format_jira_issues(issues))
