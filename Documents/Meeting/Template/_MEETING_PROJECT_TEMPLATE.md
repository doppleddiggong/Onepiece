---
title: "{{date}}_프로젝트 회의"
date: "{{date}}"
project: "{{project_name}}"
attendees:
  - "{{attendees}}"
keywords:
  - "{{keywords}}"
category: "project-meeting"   # daily와 구분되도록 고정 필드
---

# {{project_name}} — 프로젝트 회의록 ({{date}})

## 1. 개요
- **일자:** {{date}}
- **프로젝트:** {{project_name}}
- **회의 목적:** {{meeting_purpose}}
- **참석:** {{attendees}}
- **키워드:** {{keywords}}

---

## 2. 프로젝트 진행 현황 (상위 레벨)
### 2.1. 진행 상황 요약
- {{progress_summary}}

### 2.2. 주요 변경 사항(Architecture / Scenario / AI / DevOps / 클라이언트 등)
- {{major_updates}}

### 2.3. 차주 또는 마일스톤 대비 위치
- {{milestone_position}}

---

## 3. 논의 내용 (세부)
### 3.1. 기획 / 시나리오
- {{scenario_discussion}}

### 3.2. 시스템 / 아키텍처 (UE5 / 서버 / AI 구조 포함)
- {{system_arch_discussion}}

### 3.3. 기술 구현 논의 (UE5·AI·DevOps 통합)
- {{technical_discussion}}

### 3.4. 디자인 / UX / 리소스
- {{design_discussion}}

### 3.5. 일정 / 리스크 / 의사결정
- {{schedule_risk_decision}}

---

## 4. Action Items (단위별 책임·기한 명시)
### UE5 (클라이언트)
- [ ] {{client_action_1}} (담당: {{owner}}, 기한: {{due}})
- [ ] {{client_action_2}} (담당: {{owner}}, 기한: {{due}})

### AI 팀
- [ ] {{ai_action_1}} (담당: {{owner}}, 기한: {{due}})

### 기획 / PM
- [ ] {{pm_action_1}} (담당: {{owner}}, 기한: {{due}})

### DevOps / 문서 자동화
- [ ] {{devops_action_1}} (담당: {{owner}}, 기한: {{due}})

---

## 5. 의사결정(Decision Log)
> 자동화 시 Daily/Weekly DevLog와 연결될 핵심 구간  
- **결정 1:** {{decision_1}}
- **결정 2:** {{decision_2}}
- **근거:** {{decision_rationale}}

---

## 6. 리스크 / 이슈
- **기술 리스크:** {{tech_risk}}
- **일정 리스크:** {{schedule_risk}}
- **운영 리스크:** {{ops_risk}}
- **필요 조치:** {{risk_action}}

---

## 7. 참고 문서 / 산출물 링크
- 관련 Figma: {{figma_link}}
- 관련 Notion: {{notion_link}}
- 관련 GitHub: {{github_link}}
- 기타 참고 자료: {{etc_link}}

---

## 8. 회고 (팀 관점)
### 8.1. 잘된 점 (Keep)
- {{keep}}

### 8.2. 문제점 (Problem)
- {{problem}}

### 8.3. 개선/다음 단계 (Try)
- {{try}}

---
