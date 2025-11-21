#!/usr/bin/env python3
"""
회의록 로더
frontmatter가 포함된 Markdown 회의록을 파싱
"""

from pathlib import Path
import re


def parse_frontmatter(content):
    """
    Markdown 파일에서 YAML frontmatter 파싱

    Args:
        content: 파일 전체 텍스트

    Returns:
        tuple: (metadata dict, content without frontmatter)
    """
    # frontmatter 패턴: ---로 시작하고 끝남
    pattern = r'^---\s*\n(.*?)\n---\s*\n(.*)$'
    match = re.match(pattern, content, re.DOTALL)

    if not match:
        return {}, content

    frontmatter_text = match.group(1)
    body = match.group(2)

    # YAML 파싱 (간단한 버전 - python-frontmatter 없이 동작)
    metadata = {}

    current_key = None
    current_list = []

    for line in frontmatter_text.split('\n'):
        line = line.strip()
        if not line:
            continue

        # 키: 값 형식
        if ':' in line and not line.startswith('-'):
            if current_key and current_list:
                metadata[current_key] = current_list
                current_list = []

            key, value = line.split(':', 1)
            key = key.strip()
            value = value.strip().strip('"\'')

            if value:
                metadata[key] = value
                current_key = None
            else:
                # 리스트 시작
                current_key = key

        # 리스트 아이템
        elif line.startswith('-') and current_key:
            item = line[1:].strip().strip('"\'')
            current_list.append(item)

    # 마지막 리스트 처리
    if current_key and current_list:
        metadata[current_key] = current_list

    return metadata, body


def load_meeting(meeting_path):
    """
    회의록 파일 로딩

    Args:
        meeting_path: 회의록 파일 경로

    Returns:
        dict: {
            "title": str,
            "date": str,
            "attendees": list,
            "keywords": list,
            "content": str (본문)
        }
    """
    file_path = Path(meeting_path)

    if not file_path.exists():
        print(f"[WARN]  회의록 파일을 찾을 수 없습니다: {meeting_path}")
        return None

    with open(file_path, 'r', encoding='utf-8') as f:
        content = f.read()

    metadata, body = parse_frontmatter(content)

    return {
        'title': metadata.get('title', ''),
        'date': metadata.get('date', ''),
        'attendees': metadata.get('attendees', []),
        'keywords': metadata.get('keywords', []),
        'content': body.strip()
    }


def find_meeting_by_date(meeting_dir, target_date):
    """
    특정 날짜의 회의록 찾기

    Args:
        meeting_dir: 회의록 디렉토리 경로
        target_date: date 객체

    Returns:
        dict: load_meeting() 결과 또는 None
    """
    meeting_path = Path(meeting_dir)

    if not meeting_path.exists():
        print(f"[WARN]  회의록 디렉토리를 찾을 수 없습니다: {meeting_dir}")
        return None

    # 날짜 형식: Meeting_YYMMDD_Daily.md
    date_str = target_date.strftime('%y%m%d')
    pattern = f"Meeting_{date_str}_Daily.md"

    meeting_file = meeting_path / pattern

    if not meeting_file.exists():
        # 패턴이 다를 수 있으므로 전체 검색
        for file in meeting_path.glob(f"*{date_str}*.md"):
            print(f"[OK] 회의록 발견: {file.name}")
            return load_meeting(file)

        print(f"[WARN]  {target_date} 날짜의 회의록을 찾을 수 없습니다")
        return None

    return load_meeting(meeting_file)


def extract_action_items(meeting_content):
    """
    회의록에서 Action Items 섹션 추출

    Args:
        meeting_content: 회의록 본문

    Returns:
        str: Action Items 섹션 텍스트
    """
    # ## 3. Action Items 섹션 추출
    pattern = r'## 3\. Action Items\s*\n(.*?)(?=\n##|\Z)'
    match = re.search(pattern, meeting_content, re.DOTALL)

    if match:
        return match.group(1).strip()

    return ""


def extract_retrospective(meeting_content):
    """
    회의록에서 회고 섹션 추출

    Args:
        meeting_content: 회의록 본문

    Returns:
        str: 회고 섹션 텍스트
    """
    # ## 4. 회고 섹션 추출
    pattern = r'## 4\. 회고\s*\n(.*?)(?=\n##|\Z)'
    match = re.search(pattern, meeting_content, re.DOTALL)

    if match:
        return match.group(1).strip()

    return ""


if __name__ == "__main__":
    # 테스트 코드
    from datetime import date

    print("=== 회의록 로더 테스트 ===")

    # 프로젝트 루트 경로 계산
    script_dir = Path(__file__).parent
    project_root = script_dir.parent.parent.parent
    meeting_dir = project_root / "Documents" / "Meeting" / "Daily"

    print(f"회의록 디렉토리: {meeting_dir}")

    # 2025-11-20 회의록 테스트
    test_date = date(2025, 11, 20)
    meeting = find_meeting_by_date(meeting_dir, test_date)

    if meeting:
        print(f"\n제목: {meeting['title']}")
        print(f"날짜: {meeting['date']}")
        print(f"참석자: {', '.join(meeting['attendees'])}")
        print(f"키워드: {', '.join(meeting['keywords'])}")
        print(f"\n본문 (처음 200자):\n{meeting['content'][:200]}...")

        # Action Items 추출
        action_items = extract_action_items(meeting['content'])
        print(f"\nAction Items:\n{action_items[:200]}...")

        # 회고 추출
        retrospective = extract_retrospective(meeting['content'])
        print(f"\n회고:\n{retrospective[:200]}...")
    else:
        print("❌ 회의록을 찾을 수 없습니다")
