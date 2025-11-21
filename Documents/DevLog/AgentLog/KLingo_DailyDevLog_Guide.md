# 🔥 최종 통합 프롬프트 (로컬 Claude/Anthropic 용)

# 역할
당신은 DevOps 자동화 · 데이터 파이프라인 · 문서 생성 · Python 툴링을 전문으로 설계하는 엔지니어링 도우미이다.  
나는 로컬 PC(Windows + Python + uv 환경)에서 명령형으로 Python 스크립트를 반복 생성·테스트할 것이다.

# 목표
Daily DevLog 자동 생성 시스템을 로컬 Python 스크립트로 제작한다.  
GitHub Actions 없이 **로컬 실행 → 안정화 → 필요 시 GH Actions에 반영**한다.

DevLog는 다음 6가지 입력 소스를 이용해 자동 생성한다:

1) git diff (Commit Timestamp 기준: 전날 00:00 ~ 오늘 00:00)  
2) 자동 생성된 커밋 메시지 (diff 기반 GPT 커밋 메시지 생성기 존재)  
3) 매일 저장되는 회의록 (Markdown + frontmatter)  
4) 개발자 정보(developers.csv)  
5) Jira 매핑 룰(csv/yaml/json)  
6) Jira API(작업자 기반 최근 이슈 조회)

DevLog 출력은 Markdown 파일로 생성되며  
Documents/DevLog/Daily/YYYY-MM-DD.md 형태로 기록된다.

---

# 날짜 규칙 (중요)
DevLog는 **한국 시간(KST)** 기준으로 하루를 마감한다.

UTC 00:00는 KST 09:00이다.  
따라서 DevLog 생성 시점이 UTC 2025-11-21 00:00이면  
참조해야 할 날짜는 **11월 20일**이다.

Python 기준:

```

from datetime import datetime, timedelta, timezone
KST = timezone(timedelta(hours=9))
devlog_date = (now_utc.astimezone(KST).date() - timedelta(days=1))

```

---

# 파일 구조

/project
    /Documents
        /Meeting
            /Daily
                Meeting_251120_Daily.md
                Meeting_251121_Daily.md
        /DevLog
            /Daily
                2025-11-20.md
                2025-11-20.metrics.json
                2025-11-21.md
                2025-11-21.metrics.json
    /.github
        /scripts
            devlog_generate.py
            meeting_loader.py
            diff_loader.py
            jira_mapper.py
            gpt_client.py
        /data
            developers.csv
            jira_map_rules.yaml
        config.json

---

# 회의록 구조 (프론트매터 포함)

예시:

---
title: "2025-11-20_데일리 회의"
date: "2025-11-20"
attendees:
  - "전체 팀"
keywords:
  - "로컬 서버 세팅"
  - "UV 환경"
  - ...
---

# 2025-11-20 데일리 회의

## 1. 개요
...
## 2. 논의 요약
...
## 3. Action Items
...
## 4. 회고
...

Python에서 frontmatter.load() 로 metadata + content 추출.

---

# 개발자 정보 (developers.csv)

branch,developer,jira_account
dopple,배주백,jubaeck Bae
cubee,이은희,이은희
CookieAndJuice,이치훈,이치훈

브랜치명은 기능명이 아니라 **작업자명**이다.  
따라서 브랜치명 → 개발자명 → Jira ID를 매핑한다.

---

# Jira 매핑 룰 (jira_map_rules.yaml)

✔ 2) 자동 생성·갱신 방식 (너가 원하는 방식)
핵심 아이디어

로컬 Python 스크립트 하나(generate_jira_map_rules.py)를 만들어서:

Source/ 폴더 전체 스캔

최상위 모듈명 자동 분류

핵심 하위 폴더(변경률 높음, 회의록 Keywords 포함) 우선 매핑

변경된 diff에서 “자주 등장하는 폴더” 자동 탐지

GPT 호출해서 규칙 재정리

결과를 jira_map_rules.yaml로 자동 저장

✔ Python 자동 생성 스크립트 (초안)

아래 코드는 로컬에서 claude-cli로 수정하면서 진화시키면 됨.
즉, 네가 원하는 DevLog 자동화 과정의 일부로 쓰면 된다.

import os
import yaml
import subprocess
import json

TARGET_DIRS = ["Source", "Content"]

def scan_project_folders(base="."):
    modules = {}
    for root, dirs, files in os.walk(base):
        # UE5는 Source/모듈명/ 구조가 많음
        parts = root.replace("\\", "/").split("/")
        
        if len(parts) >= 2 and parts[0] in TARGET_DIRS:
            module_name = parts[1]
            path = "/".join(parts[:3])
            
            if module_name not in modules:
                modules[module_name] = set()
            modules[module_name].add(path)
    return modules

def extract_diff_paths():
    try:
        diff = subprocess.check_output(
            ["git", "diff", "--name-only", "HEAD~50..HEAD"],
            text=True
        )
    except:
        return []
    return [line.strip() for line in diff.split("\n") if line.strip()]

def build_prompt(modules, diffs):
    return f"""
다음은 Unreal Engine 프로젝트의 모듈/폴더 구조입니다:

{json.dumps(modules, indent=2)}

최근 50개 커밋의 git diff 파일 경로입니다:

{json.dumps(diffs, indent=2)}

이 데이터를 기반으로 jira_map_rules.yaml을 생성하세요.
규칙:
- 키는 시스템/모듈명
- 값은 해당 시스템과 연관된 파일 경로 prefix
- Unreal Engine 기준으로 자연스럽게 분류
- Noise는 제거
- 너무 세부 경로는 제거
"""

def call_gpt(prompt):
    from openai import OpenAI
    client = OpenAI()

    response = client.chat.completions.create(
        model="gpt-4o-mini",
        messages=[{"role": "user", "content": prompt}],
        temperature=0.2
    )
    return response.choices[0].message["content"]

def main():
    modules = scan_project_folders(".")
    diffs = extract_diff_paths()
    
    prompt = build_prompt(modules, diffs)
    yaml_text = call_gpt(prompt)

    with open("data/jira_map_rules.yaml", "w", encoding="utf-8") as f:
        f.write(yaml_text)
    
    print("✔ jira_map_rules.yaml 자동 생성 완료")

if __name__ == "__main__":
    main()

---

# GPT 분석 필요사항

GPT에게 전달되는 JSON 구조:

{
  "task": "devlog_generate",
  "date": "YYYY-MM-DD",
  "meeting": "<<회의록 본문>>",
  "keywords": [...],
  "diff": "<<git diff>>",
  "commits": "<<자동 생성된 커밋 메시지들>>",
  "mapping_rules": "<<경로-시스템 매핑 룰>>",
  "developer": "<<작업자명>>",
  "jira_recent_issues": "<<Assignee 기반 Jira 후보군 리스트>>",
  "instructions": {
    "output_format": {
      "top_changes": "string",
      "impact": "string",
      "risks": "string",
      "verification": "string",
      "meeting_todo": "string",
      "jira_candidates": "string"
    }
  }
}

---

# DevLog 최종 출력 템플릿 (Mermaid 제거)

# Daily DevLog — {{ date }}

## 1. 오늘의 주요 변경
{{ top_changes }}

## 2. 시스템 영향도
{{ impact }}

## 3. 리스크 및 문제
{{ risks }}

## 4. 검증 필요 항목
{{ verification }}

## 5. 회의 기반 후속 작업
{{ meeting_todo }}

## 6. 관련 Jira 이슈 (자동 추론)
{{ jira_candidates }}

---

# 요구사항 요약

## ✔ 로컬에서 실행 가능한 Python 코드를 생성해야 한다  
**devlog_generate.py** 스크립트를 단계적으로 구현한다.

## ✔ GitHub Actions 필요 없음  
CI는 나중 문제.

## ✔ 데이터는 CSV/YAML/MD 파일에서 로딩  
하드코딩도 허용되지만 확장 가능한 구조로.

## ✔ 시스템 영향도 / 위험도 / 검증 항목 / 회의 기반 후속 작업은 모두 GPT 기반 자동 생성  
Mermaid 구조도는 제거.

## ✔ 커밋 메시지가 매우 디테일하므로 Jira 추론 정확도를 높이는 데 적극 활용한다.

## ✔ 모든 요소는 로컬 git diff + 회의록 + Jira API 기반.

---

# 당신에게 요청하는 작업

**아래 유형의 출력물을 만들 수 있도록 Python 코드를 생성하라:**

1) meeting_loader.py  
2) diff_loader.py  
3) jira_mapper.py  
4) gpt_client.py  
5) devlog_generate.py  
6) 필요한 유틸 함수(date 계산 등)  
7) 테스트를 위한 예시 실행 코드  
8) Markdown DevLog 파일 생성 코드

각각 독립 실행 가능하며, 내가 바로 실행할 수 있어야 한다.

"전체 설계를 이해했으면 Python 코드 초안을 생성하라."  
"내가 수정 요청을 하면 다시 코드를 개선하라."

이 형태로 대화할 것이다.

```