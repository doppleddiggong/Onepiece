#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
jira_issues.json을 읽어 Epic 및 Task별 진척도를 계산하여 jira_progress.json으로 저장합니다.
"""

import sys
import json
from pathlib import Path
from datetime import datetime
from collections import defaultdict

# Windows 콘솔 인코딩 설정
if sys.platform == 'win32':
    import codecs
    sys.stdout = codecs.getwriter('utf-8')(sys.stdout.buffer, 'strict')
    sys.stderr = codecs.getwriter('utf-8')(sys.stderr.buffer, 'strict')

def calculate_progress():
    """Jira 이슈 데이터로부터 진척도를 계산합니다."""

    # 입력 파일 경로
    input_file = Path("Documents/DevLog/Metrics/jira_issues.json")

    if not input_file.exists():
        print(f"ERROR: Input file not found: {input_file}")
        print("  Please run fetch_issues.py first.")
        return 1

    try:
        # 이슈 데이터 읽기
        with open(input_file, "r", encoding="utf-8") as f:
            data = json.load(f)

        issues = data.get("issues", [])
        print(f"Processing {len(issues)} issues...")

        # Epic과 Task 분류
        epics = []
        tasks = []
        epic_tasks = defaultdict(list)  # Epic별 하위 Task 그룹화
        parent_issues = {}  # Parent 이슈 정보 저장

        # 1차: 모든 이슈를 순회하며 parent 정보 수집
        for issue in issues:
            issue_type = issue.get("type")
            issue_key = issue.get("key")

            if issue_type == "Epic":
                epics.append(issue)
            elif issue_type in ["Task", "Story", "Subtask", "Bug"]:
                tasks.append(issue)
                # Parent가 있는 경우 parent 정보 저장
                if issue.get("parent_key"):
                    parent_key = issue["parent_key"]
                    epic_tasks[parent_key].append(issue)
                    # Parent 이슈 정보가 없으면 가상 Epic 생성
                    if parent_key not in parent_issues:
                        parent_issues[parent_key] = {
                            "key": parent_key,
                            "summary": f"Parent Issue {parent_key}",
                            "status": "Unknown",
                            "type": "Epic"
                        }
                # Epic Link가 있는 경우
                elif issue.get("epic_link"):
                    epic_tasks[issue["epic_link"]].append(issue)

        # Parent 이슈를 Epic으로 추가
        for parent_key, parent_info in parent_issues.items():
            epics.append(parent_info)

        # 완료 상태 판단 함수
        def is_done(status):
            done_statuses = ["Done", "Closed", "Resolved", "Complete"]
            return status in done_statuses

        # Epic 진척도 계산
        epic_progress = []
        epic_done_count = 0

        for epic in epics:
            epic_key = epic["key"]
            epic_status = epic["status"]
            epic_is_done = is_done(epic_status)

            # Epic의 하위 Task들
            child_tasks = epic_tasks.get(epic_key, [])
            child_total = len(child_tasks)
            child_done = sum(1 for task in child_tasks if is_done(task["status"]))

            if epic_is_done:
                epic_done_count += 1

            epic_info = {
                "key": epic_key,
                "summary": epic["summary"],
                "status": epic_status,
                "is_done": epic_is_done,
                "child_tasks": child_total,
                "child_done": child_done,
                "child_progress": (child_done / child_total * 100) if child_total > 0 else 0
            }
            epic_progress.append(epic_info)

        # Task 진척도 계산
        task_total = len(tasks)
        task_done = sum(1 for task in tasks if is_done(task["status"]))

        # 전체 진척도 계산
        total_progress = {
            "epic": {
                "total": len(epics),
                "done": epic_done_count,
                "progress": (epic_done_count / len(epics) * 100) if len(epics) > 0 else 0
            },
            "task": {
                "total": task_total,
                "done": task_done,
                "progress": (task_done / task_total * 100) if task_total > 0 else 0
            }
        }

        # 결과 데이터 구성
        progress_data = {
            "calculated_at": datetime.now().isoformat(),
            "project": data.get("project"),
            "total_progress": total_progress,
            "epic_details": sorted(epic_progress, key=lambda x: x["child_progress"], reverse=True)
        }

        # 결과를 JSON 파일로 저장
        output_file = Path("Documents/DevLog/Metrics/jira_progress.json")
        with open(output_file, "w", encoding="utf-8") as f:
            json.dump(progress_data, f, indent=2, ensure_ascii=False)

        print(f"[OK] Progress calculated and saved to {output_file}")

        # 진척도 요약 출력
        print("\n=== Progress Summary ===")
        print(f"Epic: {epic_done_count}/{len(epics)} ({total_progress['epic']['progress']:.1f}%)")
        print(f"Task: {task_done}/{task_total} ({total_progress['task']['progress']:.1f}%)")

        print("\n=== Epic Details ===")
        for epic in epic_progress[:5]:  # 상위 5개만 출력
            print(f"{epic['key']}: {epic['summary'][:40]}")
            print(f"  Status: {epic['status']}, Child Tasks: {epic['child_done']}/{epic['child_tasks']} ({epic['child_progress']:.1f}%)")

        return 0

    except json.JSONDecodeError as e:
        print(f"ERROR: Failed to parse JSON file")
        print(f"  {str(e)}")
        return 1
    except Exception as e:
        print(f"ERROR: Unexpected error occurred")
        print(f"  {str(e)}")
        import traceback
        traceback.print_exc()
        return 1

if __name__ == "__main__":
    sys.exit(calculate_progress())
