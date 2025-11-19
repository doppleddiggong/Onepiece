# Meeting + DevLog Integration Guide

이 문서는 DevLog 자동화(일간/주간)와 회의록을 연결해 GPT/Agent가 실질적인 회고 자산을 만드는 흐름을 설명합니다.

## 1. 핵심 흐름 요약
1. `Documents/DevLog/Metrics/*.json`에서 수치(커밋 수, top_changes, ubt/cook/test 등)를 추출합니다.
2. 기존 `Documents/DevLog/Daily/{date}.md` 또는 `Documents/DevLog/Weekly/{range}.md`를 읽어 `update_in_place()` 방식으로 섹션만 교체합니다.
3. GPT 프롬프트는 해당 모드(`daily`/`weekly`)별 템플릿을 사용하고, “왜 중요한가”를 설명하는 문장을 반환받아 바로 기존 마크다운 섹션에 삽입합니다.
4. 회의록(`Documents/Meeting/{date}.md`)에서 키워드/Action Item을 뽑아 `## 3. 회의 연계 분석` 섹션을 자동으로 생성하거나 갱신합니다.

## 2. Daily DevLog 섹션 업데이트 대상
| 섹션 | metrics 항목 | 설명 |
| --- | --- | --- |
| `## 1. 오늘의 핵심 변경` | `top_changes`, `commit_count` | 주요 커밋 요약, 영향 파일 및 라인 변경 |
| `## 2. 시스템 영향도` | `ubt`, `cook`, `tests`, `static_analysis` | 테스트/빌드 안정성 지표 |
| `## 4. Mermaid 개요도` | commit/build/test/docs 수치 | flowchart용 수치 기반 다이어그램 |
| `## 생성 시간` | `generated_at` | 자동 생성 타임스탬프 |

없으면 템플릿 전체를 생성하고, 있으면 해당 블록만 replace합니다.

## 3. Honkit Front Matter 규칙
Honkit은 각 회의록 상단에 **YAML Front Matter** 블록(`---`으로 시작/종료)을 요구합니다.
프론트 매터가 없으면 목록(`*`, `-`) 첫 줄을 YAML 앨리어스로 해석해 아래와 같은 오류가 발생합니다.

## 3. 폴더 및 파일명 규칙

회의록은 목적에 따라 다음 두 폴더로 분류하여 저장합니다. 이 구조는 Honkit 사이드바에 자동으로 반영됩니다.

- **`Documents/Meeting/Daily/`**: 매일 정해진 시간에 진행되는 **데일리 스크럼(Daily Scrum)** 회의록만 저장합니다.
  - 파일명 예시: `Meeting_251114_Daily.md`
- **`Documents/Meeting/Common/`**: **데일리 스크럼을 제외한 모든 회의록**을 저장합니다.
  - 데일리 스크럼 이후에 진행된 개별 주제 회의, 기술 리뷰, 기획 회의, 1:1 미팅 등이 모두 여기에 해당합니다.
  - 파일명 예시: `Meeting_2511_Week2_Summary.md`, `Meeting_RAG_Tech_Review.md`


## 4. Honkit Front Matter 규칙
Honkit은 각 회의록 상단에 **YAML Front Matter** 블록(`---`으로 시작/종료)을 요구합니다.
프론트 매터가 없으면 목록(`*`, `-`) 첫 줄을 YAML 앨리어스로 해석해 아래와 같은 오류가 발생합니다.

```
YAMLException: name of an alias node must contain at least one character
```

모든 회의 파일을 다음 구조처럼 작성해 주세요.

```markdown
---
title: "251113_데일리 회의"
date: "2025-11-13"
attendees:
  - "전 팀 전체"
keywords:
  - "OT(온보딩)"
---

# 251113_데일리 회의
...
```

## 4. 회의록 연결 (Meeting Template)
다음 템플릿을 회의록으로 유지하면 자동 연결이 쉬워집니다.

```
---
title: "회의 제목"
date: "{{ date }}"
attendees:
  - "{{ attendees }}"
keywords:
  - "{{ keywords }}"
---

## 개요
- 날짜: {{ date }}
- 참석: {{ attendees }}
- 키워드: {{ keywords }}

## 논의 요약
1. {{ topic_1 }} — 담당 {{ owner_1 }} / 결정 {{ decision_1 }}

## Action Items
- [ ] {{ action_1 }} (예정일 {{ due_1 }})

## 회고
- 잘된 점: {{ wins }}
- 미진한 점: {{ blockers }}
```

`keywords`, `Action Items`을 Daily metrics summary와 비교하고 공통 키워드가 있으면 `## 3. 회의 연계 분석`을 GPT가 문장으로 작성합니다.

## 4. GPT 프롬프트 방향
* `daily` 모드는 “정량 지표 + 의미 해석 + Next”를 3단락 이내, 표/숫자 포함해 출력
* `weekly` 모드는 “패턴·추세·문서 요약·AI 분석·Next” 중심
* `meeting link` 섹션은 “회의의 action과 오늘 작업의 연결 / 미진 항목”을 기술하도록 합니다.

## 5. Discord/Workflow 알림 힌트
- Discord에는 “어떤 섹션이 갱신됐는지”(예: `Mermaid 개요도`, `회의 연계`)를 짧게 알려 주세요.
- `meeting-broadcast`는 `latest_file`을 `client_payload`로 전달해서 `honkit` notify가 회의 전용 webhook을 쓸 수 있도록 유지합니다.

## 6. 실용성 체크
1. metrics를 기반으로 매일 문서가 덧붙여지는지 확인 (`git diff Documents/DevLog`).
2. GPT summary가 중복 문장 없이 템플릿에 잘 맞는지 검토.
3. 회의 연계 분석이 실제 업무 리포트/피드백과 이어지는지 팀 피드백 확인.

이 흐름을 통해 “데이터→의미→자동화” 루프가 회고/DevLog의 중심이 됩니다.
