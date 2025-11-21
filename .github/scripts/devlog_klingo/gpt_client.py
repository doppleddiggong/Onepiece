#!/usr/bin/env python3
"""
GPT 클라이언트
OpenAI API를 사용하여 DevLog 내용 생성
"""

import json
import os
from openai import OpenAI


def load_prompt_template():
    """
    프롬프트 템플릿 파일 로딩

    Returns:
        str: 프롬프트 템플릿 또는 None
    """
    from pathlib import Path
    template_path = Path(__file__).parent / 'templates' / 'devlog_prompt.txt'

    if template_path.exists():
        return template_path.read_text(encoding='utf-8')
    return None


def create_devlog_prompt(data):
    """
    DevLog 생성을 위한 GPT 프롬프트 생성

    Args:
        data: {
            "date": "YYYY-MM-DD",
            "developer": "개발자명",
            "meeting": {
                "title": str,
                "keywords": list,
                "content": str
            },
            "diff": str,
            "commits": list,
            "changed_files": list,
            "top_systems": list,
            "jira_issues": list,
            "mapping_rules": dict
        }

    Returns:
        str: GPT 프롬프트
    """
    # 템플릿 로딩 시도
    template = load_prompt_template()

    # 커밋 메시지 포맷팅
    commit_messages = []
    for commit in data.get('commits', []):
        msg = f"[{commit['short_hash']}] {commit['subject']}"
        if commit.get('body'):
            msg += f"\n{commit['body']}"
        commit_messages.append(msg)

    commits_text = '\n\n'.join(commit_messages) if commit_messages else "커밋 없음"

    # 회의록 정보
    meeting = data.get('meeting')
    if meeting:
        meeting_text = f"""
## 회의록 정보
- 제목: {meeting.get('title', 'N/A')}
- 키워드: {', '.join(meeting.get('keywords', []))}

{meeting.get('content', '회의록 없음')[:1000]}
"""
    else:
        meeting_text = "회의록 없음"

    # 변경된 시스템
    top_systems = data.get('top_systems', [])
    systems_text = '\n'.join([f"- {system}: {count}개 파일" for system, count in top_systems]) if top_systems else "변경된 시스템 없음"

    # Jira 이슈
    jira_issues = data.get('jira_issues', [])
    if jira_issues:
        jira_text = '\n'.join([f"- [{issue['key']}] {issue['summary']} (상태: {issue['status']})" for issue in jira_issues])
    else:
        jira_text = "조회된 Jira 이슈 없음"

    # Git diff (처음 5000자만)
    diff_text = data.get('diff', '')[:5000]
    if len(data.get('diff', '')) > 5000:
        diff_text += "\n\n... (나머지 생략)"

    # 프롬프트 생성
    prompt = f"""
당신은 소프트웨어 개발 팀의 DevLog 작성을 돕는 AI 어시스턴트입니다.

다음 정보를 바탕으로 {data.get('date')} 날짜의 Daily DevLog를 작성해주세요.

# 입력 정보

## 날짜
{data.get('date')}

## 개발자
{data.get('developer', 'Unknown')}

{meeting_text}

## Git 커밋 메시지
{commits_text}

## 변경된 파일 ({len(data.get('changed_files', []))}개)
{', '.join(data.get('changed_files', [])[:20])}

## 주요 변경 시스템
{systems_text}

## 최근 Jira 이슈
{jira_text}

## Git Diff (일부)
```
{diff_text}
```

---

# 출력 형식

다음 형식으로 DevLog를 작성해주세요:

## 1. 오늘의 주요 변경
- 커밋 메시지와 diff를 기반으로 주요 변경 사항을 3~5개 항목으로 요약
- 각 항목은 기술적으로 정확하고 명확하게 작성
- 변경의 목적과 영향을 포함

## 2. 시스템 영향도
- 변경된 시스템별 영향도 분석
- 주요 모듈/컴포넌트에 미치는 영향 설명
- 시스템 간 의존성이 있다면 언급

## 3. 리스크 및 문제
- 잠재적인 문제점이나 리스크 식별
- 해결이 필요한 기술적 부채 언급
- 없다면 "특별한 리스크 없음" 명시

## 4. 검증 필요 항목
- 테스트가 필요한 항목 나열
- 검증해야 할 기능이나 통합 포인트
- 성능 측정이 필요한 부분

## 5. 회의 기반 후속 작업
- 회의록의 Action Items와 연계된 작업 진행 상황
- 회의에서 논의된 내용과 실제 구현의 연결점
- 없다면 "회의록과 직접적인 연관 없음" 명시

## 6. 관련 Jira 이슈 (자동 추론)
- 커밋 내용, 변경 파일, Jira 이슈 목록을 종합하여 관련 이슈 추론
- 각 이슈와 오늘 작업의 연관성 설명
- 형식: [이슈 키] 이슈 제목 - 연관성 설명

---

주의사항:
- 한국어로 작성
- 기술적으로 정확하고 구체적으로 작성
- 추측보다는 실제 데이터 기반으로 작성
- 마크다운 형식 준수
- Mermaid 다이어그램은 사용하지 않음
"""

    return prompt


def generate_devlog_with_gpt(data, api_key=None, model="gpt-4o"):
    """
    GPT API를 사용하여 DevLog 생성

    Args:
        data: create_devlog_prompt()에 전달할 데이터
        api_key: OpenAI API 키 (None이면 환경 변수에서 로딩)
        model: 사용할 GPT 모델

    Returns:
        str: 생성된 DevLog 본문
    """
    if api_key is None:
        api_key = os.getenv('OPENAI_API_KEY')

    if not api_key:
        raise ValueError("OpenAI API 키가 설정되지 않았습니다. OPENAI_API_KEY 환경 변수를 설정하세요.")

    # OpenAI 클라이언트 초기화
    client = OpenAI(api_key=api_key)

    # 프롬프트 생성
    prompt = create_devlog_prompt(data)

    try:
        # GPT API 호출
        response = client.chat.completions.create(
            model=model,
            messages=[
                {"role": "system", "content": "당신은 소프트웨어 개발 팀의 DevLog 작성을 돕는 전문 AI 어시스턴트입니다. 기술적으로 정확하고 명확한 문서를 작성합니다."},
                {"role": "user", "content": prompt}
            ],
            temperature=0.3,  # 일관성 있는 출력을 위해 낮은 temperature
            max_tokens=3000
        )

        # 응답 추출
        devlog_content = response.choices[0].message.content

        return devlog_content

    except Exception as e:
        print(f"[WARN]  GPT API 호출 실패: {e}")
        return None


def generate_devlog_fallback(data):
    """
    GPT API 사용 불가 시 기본 DevLog 생성

    Args:
        data: DevLog 데이터

    Returns:
        str: 기본 DevLog 본문
    """
    date = data.get('date', 'N/A')
    developer = data.get('developer', 'Unknown')
    commits = data.get('commits', [])
    changed_files = data.get('changed_files', [])
    top_systems = data.get('top_systems', [])

    # 커밋 요약
    commit_summary = []
    for commit in commits[:5]:
        commit_summary.append(f"- `{commit['short_hash']}` {commit['subject']}")

    commit_text = '\n'.join(commit_summary) if commit_summary else "- 커밋 없음"

    # 시스템 요약
    system_summary = []
    for system, count in top_systems:
        system_summary.append(f"- **{system}**: {count}개 파일 변경")

    system_text = '\n'.join(system_summary) if system_summary else "- 변경된 시스템 없음"

    # 기본 템플릿
    fallback_devlog = f"""## 1. 오늘의 주요 변경

{commit_text}

**총 {len(commits)}개 커밋, {len(changed_files)}개 파일 변경**

## 2. 시스템 영향도

{system_text}

## 3. 리스크 및 문제

- (GPT 분석 필요)

## 4. 검증 필요 항목

- 변경된 기능의 단위 테스트
- 통합 테스트 실행
- 빌드 검증

## 5. 회의 기반 후속 작업

- (회의록 분석 필요)

## 6. 관련 Jira 이슈 (자동 추론)

- (Jira API 연동 필요)

---

**[WARN] 이 DevLog는 GPT API 없이 자동 생성되었습니다. 수동으로 내용을 보완해주세요.**
"""

    return fallback_devlog


if __name__ == "__main__":
    # 테스트 코드
    print("=== GPT 클라이언트 테스트 ===")

    # 테스트 데이터
    test_data = {
        "date": "2025-11-20",
        "developer": "배주백",
        "meeting": {
            "title": "2025-11-20 데일리 회의",
            "keywords": ["로컬 서버 세팅", "UV 환경", "STT/TTS"],
            "content": "팀 전반에 걸쳐 로컬 개발 환경 구조 이해 필요..."
        },
        "commits": [
            {
                "short_hash": "abc123",
                "subject": "feat: add network data structure",
                "body": "Implement basic network data classes"
            },
            {
                "short_hash": "def456",
                "subject": "fix: resolve voice system memory leak",
                "body": ""
            }
        ],
        "changed_files": [
            "Source/Onepiece/Network/Private/NetworkData.cpp",
            "Source/Onepiece/Voice/Private/UVoiceConversationSystem.cpp"
        ],
        "top_systems": [
            ("Network", 2),
            ("Voice", 1)
        ],
        "jira_issues": [
            {
                "key": "ONE-123",
                "summary": "네트워크 데이터 구조 설계",
                "status": "In Progress"
            }
        ],
        "diff": "diff --git a/Source/Onepiece/Network/Private/NetworkData.cpp...",
        "mapping_rules": {}
    }

    # 프롬프트 생성 테스트
    print("\n=== 프롬프트 생성 테스트 ===")
    prompt = create_devlog_prompt(test_data)
    print(f"프롬프트 길이: {len(prompt)} 문자")
    print(f"\n프롬프트 (처음 500자):\n{prompt[:500]}...\n")

    # Fallback DevLog 생성 테스트
    print("=== Fallback DevLog 생성 테스트 ===")
    fallback = generate_devlog_fallback(test_data)
    print(fallback)

    # GPT API 테스트 (실제 호출은 주석 처리)
    print("\n=== GPT API 테스트 ===")
    print("[WARN]  GPT API 호출은 OPENAI_API_KEY 환경 변수 설정 후 사용하세요")
    # devlog = generate_devlog_with_gpt(test_data)
    # if devlog:
    #     print(devlog)
