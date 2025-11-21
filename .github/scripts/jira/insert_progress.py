#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
DevLog Daily 문서에 Jira 진척도와 AI 분석을 자동으로 삽입합니다.
"""

import argparse
import os
import sys
import json
import subprocess
from pathlib import Path
from datetime import datetime, timedelta, timezone

# Windows 콘솔 인코딩 설정
if sys.platform == 'win32':
    import codecs
    sys.stdout = codecs.getwriter('utf-8')(sys.stdout.buffer, 'strict')
    sys.stderr = codecs.getwriter('utf-8')(sys.stderr.buffer, 'strict')

def run_script(script_path):
    """Python 스크립트를 실행합니다."""
    result = subprocess.run(
        [sys.executable, script_path],
        capture_output=True,
        text=True,
        encoding='utf-8',
        errors='replace'  # 디코딩 오류 시 대체 문자 사용
    )
    print(result.stdout)
    if result.stderr:
        print(result.stderr, file=sys.stderr)
    return result.returncode

def get_devlog_path(target_date: str | None) -> Path:
    """Daily DevLog 파일 경로를 반환합니다."""
    if target_date:
        resolved_date = target_date
    else:
        kst = timezone(timedelta(hours=9))
        resolved_date = (datetime.now(tz=kst) - timedelta(days=1)).strftime("%Y-%m-%d")

    return Path(f"Documents/DevLog/Daily/{resolved_date}.md")


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(description="Insert Jira progress into a Daily DevLog.")
    parser.add_argument("--date", help="Target Daily DevLog date (YYYY-MM-DD). Defaults to yesterday (KST) if omitted.")
    return parser.parse_args()

def load_json_file(file_path):
    """JSON 파일을 읽어 반환합니다."""
    try:
        with open(file_path, "r", encoding="utf-8") as f:
            return json.load(f)
    except Exception as e:
        print(f"ERROR: Failed to load {file_path}: {str(e)}")
        return None

def generate_progress_section(progress_data):
    """Jira 진척도 섹션을 Markdown으로 생성합니다."""
    total = progress_data.get("total_progress", {})
    epic_info = total.get("epic", {})
    task_info = total.get("task", {})

    section = "\n---\n\n"
    section += "## 8. Jira 개발 진척도\n\n"
    section += "### 전체 진행도\n\n"
    section += f"- **Epic**: {epic_info.get('done', 0)}/{epic_info.get('total', 0)} ({epic_info.get('progress', 0):.1f}%)\n"
    section += f"- **Task**: {task_info.get('done', 0)}/{task_info.get('total', 0)} ({task_info.get('progress', 0):.1f}%)\n"

    epic_details = progress_data.get("epic_details", [])
    if epic_details:
        section += "\n### Epic 상세 진행 상황\n\n"
        for epic in epic_details[:10]:  # 상위 10개만 표시
            status_icon = "[DONE]" if epic.get("is_done") else "[WIP]"
            progress_bar = "=" * int(epic['child_progress'] / 10) + "-" * (10 - int(epic['child_progress'] / 10))
            section += f"**{status_icon} {epic['key']}**: {epic['summary']}\n"
            section += f"- Progress: [{progress_bar}] {epic['child_progress']:.1f}%\n"
            section += f"- Child Tasks: {epic['child_done']}/{epic['child_tasks']}\n\n"

    return section

def call_gpt_analysis(progress_data, issues_data, devlog_content):
    """OpenAI GPT를 호출하여 AI 분석 섹션을 생성합니다."""
    try:
        import openai
    except ImportError:
        print("WARNING: openai package not installed. Skipping AI analysis.")
        return None

    api_key = os.getenv("OPENAI_API_KEY")
    if not api_key:
        print("WARNING: OPENAI_API_KEY not set. Skipping AI analysis.")
        return None

    # GPT에게 전달할 프롬프트 구성
    today = datetime.now()
    deadline = datetime(2025, 12, 9)
    days_remaining = (deadline - today).days

    prompt = f"""당신은 게임 개발 프로젝트 관리 전문 어시스턴트입니다.
아래의 Jira 진척도 데이터를 기반으로 개발팀에게 객관적이고 실용적인 분석을 제공하세요.

## 프로젝트 정보
- 오늘 날짜: {today.strftime('%Y-%m-%d')}
- 마감일: 2025-12-09
- 남은 기간: {days_remaining}일

## Jira 진척도 데이터
```json
{json.dumps(progress_data, indent=2, ensure_ascii=False)}
```

## Jira 이슈 목록 (최근 15개)
```json
{json.dumps(issues_data.get('issues', [])[:15], indent=2, ensure_ascii=False)}
```

## 출력 요구사항

다음 구조로 Markdown 형식의 분석 결과를 작성하세요:

### AI 개발 분석

**진행 현황**
- 전체 Epic/Task 진행률 요약 (2-3줄)
- 남은 기간({days_remaining}일) 대비 현재 진행 속도 평가

**주의 항목**
- 진행률이 낮거나 지연 가능성이 있는 Epic 2-3개 (구체적 이유 포함)

**권장 사항**
- 마감일까지 완료를 위한 실질적 조언 2-3개

톤: 객관적, 냉정한 분석, 과장 없음
분량: 10-15줄
형식: 간결한 불릿 포인트
"""

    try:
        client = openai.OpenAI(api_key=api_key)
        response = client.chat.completions.create(
            model="gpt-4o-mini",
            messages=[
                {"role": "system", "content": "You are a professional game development project management assistant."},
                {"role": "user", "content": prompt}
            ],
            temperature=0.7,
            max_tokens=1000
        )

        ai_analysis = response.choices[0].message.content

        # AI 분석 섹션 포맷팅
        formatted_analysis = "\n---\n\n"
        formatted_analysis += "## 9. AI 개발 분석\n\n"
        formatted_analysis += ai_analysis.replace("### AI 개발 분석", "").strip()
        formatted_analysis += "\n"

        return formatted_analysis

    except Exception as e:
        print(f"WARNING: Failed to generate AI analysis: {str(e)}")
        return None

def insert_progress_into_devlog(target_date: str | None):
    """DevLog에 Jira 진척도를 삽입하는 메인 함수입니다."""

    print("=== Starting Jira Progress Insertion ===")
    if target_date:
        print(f"Target date provided: {target_date}")

    # 1. fetch_issues.py 실행
    print("\n[1/5] Fetching Jira issues...")
    fetch_script = Path(".github/scripts/jira/fetch_issues.py")
    if fetch_script.exists():
        result = run_script(fetch_script)
        if result != 0:
            print("ERROR: Failed to fetch Jira issues")
            return 1
    else:
        print(f"ERROR: Script not found: {fetch_script}")
        return 1

    # 2. calc_progress.py 실행
    print("\n[2/5] Calculating progress...")
    calc_script = Path(".github/scripts/jira/calc_progress.py")
    if calc_script.exists():
        result = run_script(calc_script)
        if result != 0:
            print("ERROR: Failed to calculate progress")
            return 1
    else:
        print(f"ERROR: Script not found: {calc_script}")
        return 1

    # 3. 오늘자 DevLog 파일 찾기
    print("\n[3/5] Finding today's DevLog...")
    devlog_path = get_devlog_path(target_date)

    if not devlog_path.exists():
        print(f"WARNING: DevLog file not found: {devlog_path}")
        print("  Skipping Jira progress insertion.")
        return 0  # DevLog가 없으면 실패가 아님 (정상 종료)

    # 4. 진척도 데이터 로드
    print("\n[4/5] Loading progress data...")
    progress_file = Path("Documents/DevLog/Metrics/jira_progress.json")
    issues_file = Path("Documents/DevLog/Metrics/jira_issues.json")

    progress_data = load_json_file(progress_file)
    issues_data = load_json_file(issues_file)

    if not progress_data or not issues_data:
        print("ERROR: Failed to load progress data")
        return 1

    # 5. DevLog 파일 읽기
    print("\n[5/5] Inserting progress into DevLog...")
    try:
        # UTF-8로 읽기 시도, 실패 시 다른 인코딩 시도
        try:
            with open(devlog_path, "r", encoding="utf-8") as f:
                devlog_content = f.read()
        except UnicodeDecodeError:
            print("  WARNING: UTF-8 decoding failed, trying with cp949...")
            with open(devlog_path, "r", encoding="cp949") as f:
                devlog_content = f.read()

        # 이미 Jira 진척도가 삽입되어 있는지 확인
        if "## 8. Jira 개발 진척도" in devlog_content or "## Jira 개발 진척도" in devlog_content:
            print("INFO: Jira progress section already exists. Skipping insertion.")
            return 0

        # 진척도 섹션 생성
        progress_section = generate_progress_section(progress_data)

        # AI 분석 생성 (선택사항)
        ai_section = ""
        openai_api_key = os.getenv("OPENAI_API_KEY")
        if openai_api_key:
            print("  Generating AI analysis...")
            ai_analysis = call_gpt_analysis(progress_data, issues_data, devlog_content)
            if ai_analysis:
                ai_section = f"\n## AI 개발 분석 및 피드백\n\n{ai_analysis}\n"

        # DevLog에 섹션 추가 (문서 끝에 삽입)
        updated_content = devlog_content.rstrip() + "\n" + progress_section

        if ai_section:
            updated_content += ai_section

        # 파일 저장
        with open(devlog_path, "w", encoding="utf-8") as f:
            f.write(updated_content)

        print(f"[OK] Jira progress inserted into {devlog_path}")
        return 0

    except Exception as e:
        print(f"ERROR: Failed to insert progress into DevLog")
        print(f"  {str(e)}")
        import traceback
        traceback.print_exc()
        return 1

def main() -> int:
    args = parse_args()
    return insert_progress_into_devlog(args.date)


if __name__ == "__main__":
    sys.exit(main())
