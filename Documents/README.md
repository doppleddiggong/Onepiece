# Onepiece 프로젝트 개발 문서

이 사이트는 언리얼 엔진 5.6.1과 C++로 개발 중인 **Onepiece 프로젝트** (POTENUP 최종 프로젝트)의 모든 기술 문서, 기획 자료, 개발 로그를 통합 관리하는 공간입니다.

---

## 📂 문서 구조

### 1. 📚 API 문서 (Doxygen)
C++ 소스 코드의 주석을 기반으로 자동 생성되는 API 레퍼런스입니다. 클래스, 함수, 변수 등 코드 레벨의 상세한 정보를 확인할 수 있습니다.

- **[API 문서 바로가기 &raquo;](https://doppleddiggong.github.io/Onepiece/doxygen/)**

### 2. 📝 시스템 설계 및 기획 (Planning)
주요 시스템의 아키텍처 설계, 기능 명세, 기술 연구(R&D) 등 프로젝트의 기획 및 설계와 관련된 모든 문서를 포함합니다.

- **프로젝트 단계:**
  - **기획 단계** (2025.11.18): 프로젝트 구체화 및 핵심 기능 설계
  - **프로토 단계** (2025.12.09): 핵심 기능 최소 1개 이상 실체 구현
  - **알파 단계** (2025.12.22): 각 기능들의 실체 연동
  - **베타 단계** (2026.01.05): 기술적 완성도 평가
  - **최종 발표** (2026.01.08): 기업 초청 평가

### 3. 📅 개발 로그 (DevLog)
GitHub Actions를 통해 매일 자동으로 생성되는 개발 일지입니다. 커밋 내역, 코드 변경 사항 등 일일 개발 활동이 투명하게 기록됩니다.

- **주요 기능:**
  - **Daily Log:** 매일 오전 9시(KST)에 자동으로 생성되는 일일 개발 로그
  - **Weekly Summary:** 매주 일요일 밤에 생성되는 주간 요약 리포트

### 4. 🤖 에이전트 규칙 & 워크플로우 (AgentRule)
프로젝트에서 활동하는 모든 에이전트가 따라야 할 규칙과 공용 워크플로우를 한눈에 확인할 수 있도록 정리했습니다. 자세한 설명과 최신
버전은 각 문서를 직접 확인하세요.

| 구분 | 문서 | 설명 |
| --- | --- | --- |
| 규칙 | [Onepiece Agent 공통 가이드](../AgentRule/Project/Onepiece/AGENT_GUIDE.md) | 에이전트 공통 규칙, 프로젝트 범위, 문서화 정책 |
| 규칙 | [코딩 컨벤션](../AgentRule/Project/Onepiece/CODING_CONVENTIONS.md) | Onepiece 전용 C++/언리얼 스타일, 로깅 및 매크로 정책 |
| 규칙 | [프로젝트 개요](../AgentRule/Project/Onepiece/PROJECT_OVERVIEW.md) | 프로젝트 목표, 모듈 구조, 일정 요약 |
| 규칙 | [Doxygen 설정 가이드](../AgentRule/Project/Onepiece/DOXYGEN_SETUP.md) | 문서화 파이프라인과 비공개 문서 관리 절차 |
| 워크플로우 | [커밋 메시지 작성 워크플로우](../AgentRule/Common/Workflows/commit_agent.md) | Conventional Commits 포맷, 메시지 작성 규칙 |
| 워크플로우 | [코드 생성 절차](../AgentRule/Common/Workflows/code_generate.md) | 대규모 기능 구현 시 계획·구현·검증 단계 |
| 워크플로우 | [디버깅 가이드](../AgentRule/Common/Workflows/debug_guide.md) | 버그 재현, 로깅, 원인 파악을 위한 체크리스트 |
| 워크플로우 | [에이전트 상호작용 가이드](../AgentRule/Common/Workflows/agent_interaction_guide.md) | 명확한 프롬프트 작성과 협업 커뮤니케이션 방법 |
| 워크플로우 | [Agent Log 작성 규칙](../AgentRule/Common/Workflows/agents_log.md) | AgentLog 기록 포맷, TODO 관리 방법 |

- **모든 문서 모음:** [에이전트 규칙 모음](Guide/AgentRuleIndex.md)

> 에이전트 문서는 내부 협업용으로만 제공되며, Doxygen 공개 문서에는 포함되지 않습니다.

---

## 🏗️ 프로젝트 정보

- **엔진**: Unreal Engine 5.6.1
- **언어**: C++20, Blueprint
- **모듈**:
  - `Onepiece` (메인 게임 모듈)
  - `CoffeeLibrary` (공용 유틸리티)
  - `LatteLibrary` (게임플레이 로직)
- **개발 툴**: JetBrains Rider, Visual Studio, Doxygen, Git LFS

---

## 🔗 관련 링크

- **GitHub Repository**: [doppleddiggong/Onepiece](https://github.com/doppleddiggong/Onepiece)
- **프로젝트 개요**: [PROJECT_OVERVIEW.md](../AgentRule/Project/Onepiece/PROJECT_OVERVIEW.md)
- **코딩 컨벤션**: [CODING_CONVENTIONS.md](../AgentRule/Project/Onepiece/CODING_CONVENTIONS.md)

---

**프로젝트 시작일**: 2025-11-10
**문서 업데이트일**: 2025-11-10
