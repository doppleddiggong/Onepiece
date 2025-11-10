# Project Onepiece 개요

**[Base Rules]** `Common/PERSONA.md`

*이 문서는 `Common/PERSONA.md`에 정의된 범용 에이전트 규칙을 상속받습니다. 기본 규칙이 모두 적용되며, 아래에는 Onepiece 프로젝트에만 해당하는 특화 정보가 기술되어 있습니다.*

---

## 1) 프로젝트 정보

- **프로젝트명**: Onepiece (POTENUP 최종 프로젝트 / 가칭)
- **엔진 버전**: Unreal Engine **5.6.1** (`Onepiece.uproject` 내 `EngineAssociation: "5.6"`)
- **주요 플랫폼**: C++ 기반 멀티플랫폼(주요 플랫폼: PC) 개발
- **프로젝트 기간**: 2025.11.10(월) ~ 2026.01.08(목)
- **프로젝트 목적**:
  - 언리얼 개발 파트와 AI 에이전트 개발 파트를 통합한 프로젝트
  - 사용자/비즈니스 문제 해결 중심의 실용적 프로젝트
  - 각 단계별 평가를 통한 점진적 개발
- **핵심 기능**: (추후 기획 단계에서 결정 예정)

---

## 2) 프로젝트 일정

### 단계별 일정
- **기획 단계**: 2025.11.18 (화)
  - 프로젝트 구체화 및 적절성 점검
  - 핵심 기능 설계 수립
  - 논리적 설계 및 기술적 점검

- **프로토 단계**: 2025.12.09 (화)
  - 기획한 핵심 기능 중 최소 1개 이상 실체 구현
  - 구현된 기능을 사용 가능한 수준으로 적용
  - 구현 결과물의 기획서와 충실한 호환 확인

- **알파 단계**: 2025.12.22 (월)
  - 각 기능들이 실체 연동되는 흐름으로 작동
  - 기능한치를 충점적으로 살펴보고 과정을 확인

- **베타 단계**: 2026.01.05 (월)
  - 기술적 완성도, 문제 해결력, 협업 역량, 발표 역량을 종합적으로 평가

- **최종 발표**: 2026.01.08 (목)
  - 기업 초청 평가를 통해 프로젝트의 완성도와 활용 가능성을 종합적으로 판단

---

## 3) 개발 환경 / 요구

- **UE 설정**: `IncludeOrderVersion: Unreal5_6`, `BuildSettingsVersion: V5`
- **C++ 표준**: C++20
- **IDE**: JetBrains Rider (권장), Visual Studio는 빌드 도구로서 구성
- **필수 VS 구성요소(.vsconfig)**: Windows 11 SDK 22621, VC++ 툴체인, LLVM/Clang, Unreal VS 확장, Native Game Workload

---

## 4) 사용 언어 / 미들웨어 / API

- **C++**: 게임플레이 로직, AI, 상호작용, 이동 시스템 등 (추후 구체화)
- **블루프린트**: UI, 시네마틱, 레벨 스크립트 등 빠른 반복이 필요한 영역
- **UI**: UMG / Slate
- **외부 API**: (추후 기획 단계에서 결정 예정)

---

## 5) 모듈 구조

### 5-1) 게임 모듈: `Onepiece` (Runtime)
- **PublicDependencies**: `Core`, `CoreUObject`, `Engine`, `InputCore`, `EnhancedInput`, `AIModule`, `NavigationSystem`, `HTTP`, `Json`, `JsonUtilities`, `CoffeeLibrary`, `LatteLibrary` (필요시 추가)
- **주요 서브클래스**: (추후 기획 단계에서 구체화)
- **로그 카테고리**: `LogOnepiece` (`DECLARE_LOG_CATEGORY_EXTERN` / `DEFINE_LOG_CATEGORY`)

### 5-2) 서브 모듈: `CoffeeLibrary` (Runtime)
- **역할**: 공용 유틸리티, 데이터 처리, 네트워크/시스템 공통 로직 제공
- **네트워크 로깅**: `NETWORK_LOG` 카테고리는 `CoffeeLibrary/Public/Shared/NetworkLog.h`에 정의되어 있으므로 필요 시 확장

### 5-3) 서브 모듈: `LatteLibrary` (Runtime)
- **역할**: 이동·전투·타격감·군중 제어 등 게임플레이 핵심 로직 제공 (필요시 확장)

### 5-4) 에디터 플러그인: `CoffeeToolbar` (Editor, Win64)
- **역할**: 레벨 디자이너/프로그래머를 위한 툴바 및 자동화 기능
- **경로**: `Plugins/CoffeeToolbar/*`

---

## 6) 핵심 시스템 (추후 기획 단계에서 구체화)

현재는 YiSan 프로젝트 구조를 참고하여 구성되어 있으나, 기획 단계 이후 프로젝트 방향성에 맞게 재구성될 예정입니다.

---

## 7) 빌드/타겟 설정

- **타겟**: `OnepieceEditorTarget`(Editor), `OnepieceTarget`(Game) / Win64
- **네트워크 옵션**: (추후 결정)

---

## 8) 입력 / 맵 / 게임모드 기본값

- **Enhanced Input 사용**: `DefaultPlayerInputClass = EnhancedPlayerInput`, `DefaultInputComponentClass = EnhancedInputComponent`
- **기본 맵**: (추후 기획 단계에서 설정)
- **기본 게임모드**: (추후 기획 단계에서 설정)

---

## 9) 아트/에셋 파이프라인

- **콘텐츠 경로**: `Content/CustomContents/Assets`
- **Characters**: (추후 기획 단계에서 결정)
- **Environments**: (추후 기획 단계에서 결정)
- **Animations**: (추후 기획 단계에서 결정)
- **대용량 에셋 관리**: Git LFS 사용

---

## 10) 빌드 / 실행

- **솔루션 파일**: `Onepiece.sln` (UE 5.6.1)
- **Windows 빌드 예시**:
  ```bat
  "<UE_ROOT>\Engine\Build\BatchFiles\Build.bat" OnepieceEditor Win64 Development -Project="<PROJECT_PATH>\Onepiece.uproject" -WaitMutex
  "<UE_ROOT>\Engine\Build\BatchFiles\Build.bat" Onepiece Win64 Development -Project="<PROJECT_PATH>\Onepiece.uproject" -WaitMutex
  ```

---

## 11) 네트워크 로깅 정책

- 네트워크 연동 시에는 `NETWORK_LOG` 카테고리를 사용하여 요청/응답을 각각 `[REQ]`, `[RES]` 포맷으로 남긴다
- `NETWORK_LOG` 카테고리는 `CoffeeLibrary/Public/Shared/NetworkLog.h`에 정의되어 있으므로 필요 시 확장

---

## 12) Agent QA 로그 정책

- 프로젝트 관련 Q&A는 `Documents/AgentQA/`에 기록한다
  - Markdown 기록: `Documents/AgentQA/YYYY-MM-DD.md`
  - JSONL 기록: `Documents/AgentQA/qa_log.jsonl`
- 기록 자동화 스크립트(Windows): `Tools/save_agent_qa.ps1`
- 에이전트 작업 시작 전 확인 사항:
  - 신규/변경된 요구사항, 의사결정, 회의록을 우선 검토
  - `Documents/DevLog/_Last30Summary.md` 와 최신 일자 로그 확인
  - 민감 데이터(보안, 개인정보, 계약 정보)를 포함하거나 노출하지 않았는지
  - 문서 분량은 4~8줄 내외로 간결하게 유지하되 핵심 결정은 빠짐없이 기술한다
  - 태그 예시: Onepiece, CoffeeLibrary, LatteLibrary, Character, AI, UI, Build, Perf, Bug, Decision 등

---

## 13) 프로젝트 평가 기준

### 기획 단계 (20점)
1. **기획의 적절성** (20점)
   - 프로젝트 주제가 명확하게 정의되어 있는가
   - 해결하고자 하는 문제의 의도와 필요성이 분명한가
   - 주제와 목표가 개발 기간 내 실현 가능한 수준으로 설정되어 있는가
   - 완성 가능한 범위 내에서 구체적인 계획이 수립되어 있는가

2. **필수 기능 설계** (30점)
   - **언리얼 트랙**: 클라이언트 기능 설계, 1개 이상의 캐릭터 및 상호작용 시스템 구조 제시, AI 기능이 포함된 인터랙션 콘텐츠 조율 (UI 포함)
   - **멀티플레이 트랙**: 층 생성 및 장애, 협동 기반 멀티플레이 콘텐츠
   - **AI 에이전트 트랙**: 콘텐츠 내부 AI 기능(최소 2개 이상), LLM 활용한 콘텐츠, 챗봇, RAG 등, STI/TTS, 이미지 생성(comfyUI, Flux 등), 추천 시스템, 콘텐츠 제작 도구 AI 에이전트(게임 기획 및 최의 요약 등), 형상 생성 Tool AI 에이전트(시네마틱), 공간 생성 도구 AI 에이전트(3D 오브젝트 또는 공간 생성), 콘텐츠 도구 AI 에이전트(퀘스트, NPC 멈 등 콘텐츠 제작/설명 생성)

3. **융합 구조 설계** (30점)
   - 언리얼 트랙과 AI 에이전트 트랙 간의 협업 구조가 구체적으로 정의되어 있는가
   - 데이터 흐름 및 통신 구조가 논리적으로 설계되어 있는가
   - 양 파트의 역할 분담 방식이 명확하게 표현되어 있는가

4. **사용자 / 비즈니스 기여 가능성** (20점)
   - 프로젝트가 사용자 관점에서 실용적이고 활용 가능성이 있는가
   - 향후 확장 가능성이나 비즈니스 적용 가능성이 있는가
   - 명확한 타겟 사용자 및 사용 시나리오가 제시되어 있는가

---

## 관련 문서

- **코딩 컨벤션**: `Project/Onepiece/CODING_CONVENTIONS.md` (Onepiece 프로젝트별 네트워크/모듈 규칙)
- **범용 언리얼 규칙**: `Project/ue_coding_conventions.md`
- **Doxygen 설정**: `Project/Onepiece/DOXYGEN_SETUP.md`
