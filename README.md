# KLingo — 실전 한국어를 배우는 AI 메타버스

<p align="center">
  <a href="https://www.unrealengine.com/">
    <img src="https://img.shields.io/badge/Unreal_Engine-5.x-blue.svg" alt="UE5">
  </a>
  <a href="https://isocpp.org/">
    <img src="https://img.shields.io/badge/C++-20-blue.svg" alt="C++20">
  </a>
  <a href="https://<Your-GitHub-Username>/KLingo/doxygen/">
    <img src="https://img.shields.io/badge/API_Docs-Doxygen-blue.svg" alt="API Docs">
  </a>
  <a href="https://<Your-GitHub-Username>/KLingo/docs/">
    <img src="https://img.shields.io/badge/개발_문서-웹사이트-orange.svg" alt="Dev Docs">
  </a>
</p>

> **Unreal Engine 5 기반 실시간 상호작용 학습 시스템**
> AI는 판단을 보조하고, 모든 게임 상태와 실행은 언리얼 클라이언트가 제어합니다.

---

<p align="center">
      <a href="https://www.youtube.com/watch?v=_DqL8BV1O6Y">
        <img src="Documents/Preference/concept/concept_space4.png"
             width="600"
             alt="KLingo Title">
      </a>
      <br/>
      <sub>이미지를 클릭하면 최종 시연 영상으로 이동합니다</sub>
    </p>



## 📌 프로젝트 개요

`KLingo`는 언리얼 엔진 5로 구현된 공항 입국 환경에서
외국인 학습자가 **읽기 · 듣기 · 쓰기 · 말하기**를 실제 상황처럼 경험하도록 설계된
**실시간 상호작용 중심 학습 콘텐츠**입니다.

본 프로젝트에서 AI는 게임을 직접 제어하지 않으며,
**사용자 입력을 해석해 구조화된 결과를 반환하는 보조 시스템**으로만 사용됩니다.
모든 게임 규칙, 상태 전이, NPC 행동 결정은 **Unreal Engine 클라이언트에서 수행**됩니다.

## 🧱 Architecture Overview

KLingo는 Unreal Engine의 Gameplay Framework와 모듈 시스템을 기반으로,
게임플레이, UI, AI 연동, 네트워크를 명확히 분리한 구조로 설계되었습니다.

- Gameplay Framework(GameMode, GameState, PlayerState)를 중심으로
  모든 게임 규칙과 상태 전이를 관리
- AI 시스템은 외부 비결정적 판단 컴포넌트로 분리하고,
  Unreal Engine 내부의 사전 정의된 Gameplay Action으로만 반영
- 공통 기능과 게임플레이 로직을 계층형 모듈(CoffeeLibrary / LatteLibrary)로 분리하여
  유지보수성과 확장성을 확보

---

## ✨ 핵심 특징

* **실시간 상호작용 중심 설계**
  플레이어 입력 → 검증 → 상태 전이 → 피드백까지 모든 흐름을 UE5 Gameplay Framework 기반으로 처리

* **결정적 게임 플레이 보장**
  비결정적인 AI 결과를 사전 정의된 Gameplay Action으로 매핑하여
  네트워크·프레임 안정성을 유지

* **확장 가능한 구조**
  AI, 네트워크, UI 시스템을 명확히 분리하여
  싱글플레이 → 멀티플레이 → 서비스 확장 가능 구조 확보

---

## 🧠 AI 연동 아키텍처 개요

```text
Player Input
   ↓
Unreal Engine (Validation / State Control)
   ↓
AI Server (Analysis / Evaluation)
   ↓
Structured Result
   ↓
Unreal Engine (Gameplay Action Mapping)
```

* AI는 **판단 결과만 반환**
* 행동 실행, 상태 변경, UI 반영은 **항상 UE에서 수행**
* AI 응답 지연·실패가 게임 플레이에 영향을 주지 않도록 비동기 구조 적용

---

## 🎯 핵심 기술 과제 및 해결

### 1. AI 연동 환경에서도 흔들리지 않는 게임 플레이

**문제**

* AI 응답은 비결정적이며 지연·실패 가능성 존재

**해결**

* AI 결과를 UE 내부의 사전 정의된 Gameplay State / Action으로 변환
* 지연·실패 시에도 플레이 루프가 유지되도록 Fallback 처리

**결과**

* AI 유무와 관계없이 동일한 게임 규칙과 플레이 경험 유지
* 멀티플레이 및 서비스 환경에서도 확장 가능한 구조 확보

---

### 2. 실시간 음성·텍스트 입력을 처리하는 클라이언트 구조

**문제**

* STT/OCR 처리 시간 불확실로 프레임 드랍·입력 블로킹 위험

**해결**

* 모든 AI 요청을 **비동기 처리**
* 입력·대기·결과 수신 상태를 명확히 분리 관리

**결과**

* 입력 대기 중에도 플레이어 제어 및 UI 반응 유지
* 실시간 콘텐츠에서도 프레임 안정성 확보

---

## 🌐 네트워크 및 멀티플레이 구조

* **Listen Server 기반 멀티플레이**
* Gameplay State는 서버 권한 기반으로 관리
* AI 결과는 서버에서 검증 후 클라이언트에 반영 가능하도록 설계
* Join-in-Progress 환경에서도 상태 동기화 유지

---

## 🛠️ 기술 스택 (요약)

| 구분            | 기술                                             |
| ------------- | ---------------------------------------------- |
| Engine        | Unreal Engine 5                                |
| Language      | C++20                                          |
| Gameplay      | GameMode / GameState / PlayerState / Subsystem |
| Networking    | Listen Server, Replication                     |
| AI (External) | STT, TTS, LLM, RAG 기반 분석 서버                    |
| Database      | PostgreSQL (pgvector), Redis                   |
| Automation    | GitHub Actions, Doxygen, HonKit                |

> AI 기술 상세는 **구현 수단**이며,
> 본 프로젝트의 핵심은 **언리얼 클라이언트 아키텍처와 실행 책임 분리**입니다.

---

## ⚙️ 개발 및 문서화 자동화

* C++ API 문서 자동 생성 (Doxygen)
* 개발 문서 웹 배포 (HonKit)
* DevLog 자동 수집 및 정리
* 반복 작업 최소화를 위한 빌드·문서 파이프라인 구축

---

## 🔗 참고 자료

* 📚 개발 문서: [https://doppleddiggong.github.io/Onepiece/honkit/](https://doppleddiggong.github.io/Onepiece/honkit/)
* 🔍 API 문서: [https://doppleddiggong.github.io/Onepiece/doxygen/](https://doppleddiggong.github.io/Onepiece/doxygen/)
* 📄 최종 발표 자료:
  `Documents/Planning/Presentation/KLingo_최종 발표.pdf`

---