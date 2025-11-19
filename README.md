# KLingo - AI 기반 영어 학습 게임

<p align="center">
  <a href="https://www.unrealengine.com/">
    <img src="https://img.shields.io/badge/Unreal_Engine-5.6.1-blue.svg" alt="UE5.6.1">
  </a>
  <a href="https://isocpp.org/">
    <img src="https://img.shields.io/badge/C++-20-blue.svg" alt="C++20">
  </a>
  <a href="https://doppleddiggong.github.io/Onepiece/doxygen/index.html">
    <img src="https://img.shields.io/badge/API_Docs-Doxygen-blue.svg" alt="API Docs">
  </a>
  <a href="https://doppleddiggong.github.io/Onepiece/docs/index.html">
    <img src="https://img.shields.io/badge/개발_문서-웹사이트-orange.svg" alt="Dev Docs">
  </a>
  <a href="LICENSE">
    <img src="https://img.shields.io/badge/License-MIT-green.svg" alt="License">
  </a>
</p>

<p align="center">
  <a href="https://youtu.be/iJ_77rZhphI">
    <img src="https://github.com/doppleddiggong/Onepiece/blob/main/Documents/Preference/book1.png?raw=true"
         width="500"
         alt="Onepiece Game">
  </a>
  <br/>
  <sub>이미지를 클릭하면 시연 영상으로 이동합니다</sub>
</p>

---

## 📖 프로젝트 소개

**KLingo**는 LLM(대규모 언어 모델) 기반 AI 튜터를 활용한 협동 멀티플레이 영어 학습 게임입니다.
Unreal Engine 5.6.1과 C++20을 기반으로 개발되며, 사용자 맞춤형 학습 경험과 실시간 협동 플레이를 통해 효과적인 영어 학습 환경을 제공합니다.

**POTENUP 최종 프로젝트** (2025.11.10 ~ 2026.01.08)

---

## 🎯 프로젝트 목표

- **사용자/비즈니스 문제 해결**: 전통적인 영어 학습의 지루함을 게임과 AI 튜터를 통해 해결
- **언리얼 개발 파트와 AI 에이전트 파트의 유기적 통합**: 게임 플레이와 AI 기반 학습 시스템의 완벽한 조화
- **단계별 평가를 통한 점진적 개발**: 기획 → 프로토 → 알파 → 베타 → 최종 발표

---

## ✨ 핵심 기능

### 🤖 AI 기반 학습 시스템
- **LLM 튜터**: 실시간 질문 응답 및 학습 가이드 제공
- **단어 설명 시스템**: 클릭 한 번으로 단어 뜻/발음/예시 제공
- **개별/전체 튜터**: 개인 학습과 팀 가이드를 모두 지원
- **캐싱 전략**: LLM API 호출 최적화로 비용 절감

### 🎮 게임플레이
- **협동 멀티플레이**: 호스트-게스트 구조의 실시간 협동 플레이
- **퀘스트 시스템**: 시나리오 기반 학습 미션
- **성적 평가**: S/A/B/C 등급 기반 성취도 측정
- **보상 시스템**: 골드/레벨 보상 및 아이템 해금

### 📊 사용자 맞춤 경험
- **질문지 시스템**: 사용자 관심사 수집 및 맞춤형 콘텐츠 제공
- **광고 연동**: 관심사 기반 개인화 광고
- **상점 시스템**: 골드를 활용한 아이템 구매 (향후 구현)

---

## 🛠️ 기술 스택

### 게임 엔진 & 언어
- **Unreal Engine**: 5.6.1
- **C++**: C++20 표준
- **Blueprint**: UI, 시네마틱, 레벨 스크립트

### 주요 기술
- **멀티플레이**: UE Replication System (호스트-게스트)
- **AI 통합**: LLM API, 캐싱, 실시간 응답
- **네트워킹**: HTTP, JSON, REST API
- **UI**: UMG (Unreal Motion Graphics)

### 개발 도구
- **IDE**: JetBrains Rider, Visual Studio
- **문서화**: Doxygen, HonKit
- **버전 관리**: Git, Git LFS
- **CI/CD**: GitHub Actions
- **자동화**: DevLog, PR 리뷰, 문서 배포

---

## 📁 프로젝트 구조

```
Onepiece/
├── Source/
│   ├── Onepiece/           # 메인 게임 모듈
│   ├── CoffeeLibrary/      # 공용 유틸리티 (네트워크, 시스템)
│   └── LatteLibrary/       # 게임플레이 로직 (이동, 전투, AI)
├── Plugins/
│   └── CoffeeToolbar/      # 에디터 플러그인 (자동화 도구)
├── Content/
│   └── CustomContents/     # 게임 에셋 (캐릭터, 환경, 애니메이션)
├── Documents/              # 개발 문서
│   ├── DevLog/            # 일일/주간 개발 로그
│   ├── Meeting/           # 회의록
│   └── Planning/          # 기획 및 설계 문서
├── AgentRule/             # AI 에이전트 가이드
│   ├── PERSONA.md         # 에이전트 페르소나 정의
│   └── Project/Onepiece/  # 프로젝트 특화 규칙
└── Tools/                 # 개발 자동화 스크립트
```

---

## 🚀 시작하기

### 필수 요구사항

- **Unreal Engine**: 5.6.1 이상
- **Visual Studio**: 2022 이상
  - Windows 11 SDK (22621)
  - MSVC v143 이상
  - C++ Desktop Development Workload
- **JetBrains Rider**: 2024.x (권장)
- **Git LFS**: 대용량 에셋 관리용

### 설치 및 빌드

1. **저장소 클론**
   ```bash
   git clone https://github.com/doppleddiggong/Onepiece.git
   cd Onepiece
   ```

2. **Git LFS 설정**
   ```bash
   git lfs install
   git lfs pull
   ```

3. **프로젝트 파일 생성**
   - `Onepiece.uproject` 우클릭
   - "Generate Visual Studio project files" 선택

4. **빌드 및 실행**
   - **Rider**: `Onepiece.sln` 열기 → Build → Run
   - **Visual Studio**: `Onepiece.sln` 열기 → F5 (Build & Run)
   - **커맨드라인**:
     ```bash
     "<UE_ROOT>\Engine\Build\BatchFiles\Build.bat" OnepieceEditor Win64 Development -Project="<PROJECT_PATH>\Onepiece.uproject"
     ```

---

## 📚 문서

- **[개발 문서 웹사이트](https://doppleddiggong.github.io/Onepiece/docs/)**: DevLog, 회의록, 기획 문서
- **[API 문서 (Doxygen)](https://doppleddiggong.github.io/Onepiece/doxygen/)**: C++ 클래스 및 함수 레퍼런스
- **[프로젝트 개요](AgentRule/Project/Onepiece/PROJECT_OVERVIEW.md)**: 상세 프로젝트 정보
- **[코딩 컨벤션](AgentRule/Project/Onepiece/CODING_CONVENTIONS.md)**: C++ 스타일 가이드
- **[에이전트 가이드](AgentRule/Project/Onepiece/AGENT_GUIDE.md)**: AI 에이전트 협업 규칙

---

## 📅 프로젝트 일정

| 단계 | 날짜 | 목표 |
|------|------|------|
| **기획 단계** | 2025.11.18 | 프로젝트 구체화 및 핵심 기능 설계 |
| **프로토 단계** | 2025.12.09 | 핵심 기능 최소 1개 이상 실체 구현 |
| **알파 단계** | 2025.12.22 | 각 기능들의 실체 연동 |
| **베타 단계** | 2026.01.05 | 기술적 완성도 평가 |
| **최종 발표** | 2026.01.08 | 기업 초청 평가 |

---

## ⚙️ 자동화된 개발 워크플로우

- **C++ API 문서 자동 생성**: Doxygen을 통한 GitHub Pages 배포
- **개발 문서 웹사이트 자동 배포**: HonKit 기반 자동 빌드
- **Daily DevLog 자동 수집**: GitHub Actions를 통한 일일 커밋 로그 정리
- **Weekly Summary 생성**: GPT-4 기반 주간 개발 요약 및 회고
- **Pull Request 자동 리뷰**: AI 기반 코드 리뷰 및 피드백
- **Discord 알림**: 회의록, DevLog 업데이트 실시간 알림

---

## 👥 팀 구성

### 언리얼 개발 파트
- 클라이언트 기능 설계 및 구현
- 캐릭터 시스템 및 상호작용
- 멀티플레이 아키텍처
- UI/UX 설계

### AI 에이전트 파트
- LLM 기반 튜터 시스템
- 질문지 생성 로직
- 단어 설명 API
- 캐싱 및 최적화 전략

---

## 🤝 기여하기

이 프로젝트는 POTENUP 최종 프로젝트로, 현재 팀 내부 개발만 진행 중입니다.
프로젝트 완료 후 오픈소스 기여 가이드라인을 추가할 예정입니다.

---

## 📄 라이선스

이 프로젝트는 MIT 라이선스를 따릅니다. 자세한 내용은 [LICENSE](LICENSE) 파일을 참조하세요.

---

## 📞 연락처

- **GitHub Repository**: [doppleddiggong/Onepiece](https://github.com/doppleddiggong/Onepiece)
- **Issue Tracker**: [GitHub Issues](https://github.com/doppleddiggong/Onepiece/issues)

---

<p align="center">
  Made with ❤️ by KLingo Team
  <br/>
  <sub>POTENUP Final Project (2025.11.10 ~ 2026.01.08)</sub>
</p>
