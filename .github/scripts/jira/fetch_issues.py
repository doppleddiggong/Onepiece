#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Jira API를 사용하여 프로젝트 이슈 목록을 가져와 JSON으로 저장합니다.
"""

import os
import sys
import json
import requests
from pathlib import Path
from datetime import datetime

# Windows 콘솔 인코딩 설정
if sys.platform == 'win32':
    import codecs
    sys.stdout = codecs.getwriter('utf-8')(sys.stdout.buffer, 'strict')
    sys.stderr = codecs.getwriter('utf-8')(sys.stderr.buffer, 'strict')

def fetch_jira_issues():
    """Jira API를 통해 프로젝트의 모든 이슈를 가져옵니다."""

    # 환경 변수에서 설정 읽기
    jira_site = os.getenv("JIRA_SITE")
    jira_email = os.getenv("JIRA_EMAIL")
    jira_project = os.getenv("JIRA_PROJECT")
    jira_api_token = os.getenv("JIRA_API_TOKEN")

    # 필수 환경 변수 검증
    if not all([jira_site, jira_email, jira_project, jira_api_token]):
        print("ERROR: Required environment variables are missing:")
        print(f"  JIRA_SITE: {'✓' if jira_site else '✗'}")
        print(f"  JIRA_EMAIL: {'✓' if jira_email else '✗'}")
        print(f"  JIRA_PROJECT: {'✓' if jira_project else '✗'}")
        print(f"  JIRA_API_TOKEN: {'✓' if jira_api_token else '✗'}")
        sys.exit(1)

    # Jira API URL (새로운 JQL 엔드포인트)
    base_url = f"https://{jira_site}.atlassian.net"
    api_url = f"{base_url}/rest/api/3/search/jql"

    # API 인증 정보
    auth = (jira_email, jira_api_token)

    # JQL 쿼리: 프로젝트의 모든 이슈 가져오기
    jql = f"project = {jira_project} ORDER BY created DESC"

    # API 요청 헤더
    headers = {
        "Accept": "application/json",
        "Content-Type": "application/json"
    }

    # API 요청 바디
    payload = {
        "jql": jql,
        "maxResults": 100,
        "fields": ["summary", "status", "issuetype", "parent", "customfield_10014"]
    }

    try:
        print(f"Fetching issues from Jira project: {jira_project}")
        response = requests.post(api_url, auth=auth, headers=headers, json=payload)
        response.raise_for_status()

        data = response.json()
        issues = data.get("issues", [])

        print(f"Found {len(issues)} issues")

        # 이슈 데이터 변환 및 parent 키 수집
        processed_issues = []
        parent_keys = set()

        for issue in issues:
            fields = issue.get("fields", {})
            issue_type = fields.get("issuetype", {}).get("name", "Unknown")
            status = fields.get("status", {}).get("name", "Unknown")
            summary = fields.get("summary", "No summary")

            # Epic Link 처리
            epic_link = fields.get("customfield_10014")
            parent_key = None
            if fields.get("parent"):
                parent_key = fields["parent"]["key"]
                parent_keys.add(parent_key)

            processed_issue = {
                "key": issue["key"],
                "summary": summary,
                "type": issue_type,
                "status": status,
                "parent_key": parent_key,
                "epic_link": epic_link,
                "created": fields.get("created"),
                "updated": fields.get("updated")
            }
            processed_issues.append(processed_issue)

        # Parent 이슈들을 별도로 가져오기
        if parent_keys:
            print(f"\nFetching {len(parent_keys)} parent issues...")
            parent_jql = f"key in ({','.join(parent_keys)})"
            parent_payload = {
                "jql": parent_jql,
                "maxResults": len(parent_keys),
                "fields": ["summary", "status", "issuetype"]
            }

            try:
                parent_response = requests.post(api_url, auth=auth, headers=headers, json=parent_payload)
                parent_response.raise_for_status()
                parent_data = parent_response.json()
                parent_issues = parent_data.get("issues", [])

                print(f"Found {len(parent_issues)} parent issues")

                # Parent 이슈들을 processed_issues에 추가
                for parent_issue in parent_issues:
                    parent_fields = parent_issue.get("fields", {})
                    processed_issues.append({
                        "key": parent_issue["key"],
                        "summary": parent_fields.get("summary", "No summary"),
                        "type": parent_fields.get("issuetype", {}).get("name", "Epic"),
                        "status": parent_fields.get("status", {}).get("name", "Unknown"),
                        "parent_key": None,
                        "epic_link": None,
                        "created": parent_fields.get("created"),
                        "updated": parent_fields.get("updated")
                    })
            except Exception as e:
                print(f"WARNING: Failed to fetch parent issues: {str(e)}")

        # 결과를 JSON 파일로 저장
        output_dir = Path("Documents/DevLog/Metrics")
        output_dir.mkdir(parents=True, exist_ok=True)
        output_file = output_dir / "jira_issues.json"

        output_data = {
            "fetched_at": datetime.now().isoformat(),
            "project": jira_project,
            "total_issues": len(processed_issues),
            "issues": processed_issues
        }

        with open(output_file, "w", encoding="utf-8") as f:
            json.dump(output_data, f, indent=2, ensure_ascii=False)

        print(f"[OK] Issues saved to {output_file}")

        # 이슈 타입별 통계 출력
        type_counts = {}
        for issue in processed_issues:
            issue_type = issue["type"]
            type_counts[issue_type] = type_counts.get(issue_type, 0) + 1

        print("\nIssue type summary:")
        for issue_type, count in sorted(type_counts.items()):
            print(f"  {issue_type}: {count}")

        return 0

    except requests.exceptions.RequestException as e:
        print(f"ERROR: Failed to fetch issues from Jira")
        print(f"  {str(e)}")
        if hasattr(e.response, 'text'):
            print(f"  Response: {e.response.text}")
        return 1
    except Exception as e:
        print(f"ERROR: Unexpected error occurred")
        print(f"  {str(e)}")
        return 1

if __name__ == "__main__":
    sys.exit(fetch_jira_issues())
