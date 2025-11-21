# KLingo - AI 기반 영어 학습 게임 API 문서

<div align="center">

![Unreal Engine 5.6.1](https://img.shields.io/badge/Unreal_Engine-5.6.1-blue.svg)
![C++20](https://img.shields.io/badge/C++-20-blue.svg)
![License MIT](https://img.shields.io/badge/License-MIT-green.svg)

**Unreal Engine 5.6.1 기반 협동 멀티플레이 영어 학습 게임**

[GitHub Repository](https://github.com/doppleddiggong/Onepiece) | [개발 문서](https://doppleddiggong.github.io/Onepiece/honkit/)

</div>

---

## 📖 프로젝트 소개

**KLingo**는 LLM(대규모 언어 모델) 기반 AI 튜터를 활용한 협동 멀티플레이 영어 학습 게임입니다.

Unreal Engine 5.6.1과 C++20을 기반으로 개발되며, 사용자 맞춤형 학습 경험과 실시간 협동 플레이를 통해 효과적인 영어 학습 환경을 제공합니다.

### 핵심 기능

- **LLM 기반 AI 튜터**: 실시간 질문 응답, 학습 가이드, 단어 설명 시스템
- **협동 멀티플레이**: 호스트-게스트 구조의 실시간 협동 플레이
- **퀘스트 시스템**: 시나리오 기반 학습 미션 및 성적 평가
- **개인화 학습**: 질문지 시스템을 통한 맞춤형 콘텐츠 제공

---

## 🏗️ 프로젝트 구조

본 프로젝트는 세 가지 주요 모듈로 구성되어 있습니다:

### 📦 메인 모듈: Onepiece

게임의 핵심 로직과 시스템을 담당하는 메인 게임 모듈입니다.

- **Character**: 플레이어 컨트롤, 이동 시스템, 타격감 시스템
- **Network**: HTTP/WebSocket 통신, 네트워크 설정
- **UI**: 다이얼로그, 로딩 UI 관리

주요 클래스: \ref APlayerControl, \ref APlayerActor, \ref UHttpNetworkSystem, \ref UWebSocketSystem

### ☕ 유틸리티 라이브러리: CoffeeLibrary

공용 유틸리티 및 시스템 레벨 기능을 제공하는 라이브러리입니다.

- **Actor**: 액터 관리, 애니메이션, 궤도 움직임
- **Features**: 수학 함수, Easing 함수, 포물선 계산
- **Shared**: 헬퍼 클래스, 로깅 시스템, 색상 정의

주요 클래스: \ref UEaseFunctionLibrary, \ref UCommonFunctionLibrary, \ref FLogWriter

### ☕ 게임플레이 라이브러리: LatteLibrary

게임플레이 핵심 로직과 데이터 관리를 담당하는 라이브러리입니다.

- **Character**: 시야 시스템, 캐릭터 동작
- **Data**: 캐릭터 정보, VFX/사운드 데이터, 데미지 타입
- **Manager**: 게임 데이터, VFX, 사운드, 오브젝트 풀 관리

주요 클래스: \ref UGameDataManager, \ref UGameVFXManager, \ref UGameSoundManager

---

## 🛠️ 기술 스택

### 게임 엔진 & 언어
- **Unreal Engine**: 5.6.1
- **C++**: C++20 표준
- **Blueprint**: UI, 시네마틱, 레벨 스크립트

### 주요 기술
- **멀티플레이**: UE Replication System (호스트-게스트)
- **AI 통합**: LLM API, 캐싱, 실시간 응답
- **네트워킹**: HTTP, JSON, REST API, WebSocket
- **UI**: UMG (Unreal Motion Graphics)

---

## 📚 문서 안내

### API 문서 탐색

- **[네임스페이스 목록](namespaces.html)**: 프로젝트의 모든 네임스페이스 확인
- **[클래스 목록](annotated.html)**: 모든 클래스와 구조체 확인
- **[클래스 계층도](hierarchy.html)**: 상속 관계 시각화
- **[파일 목록](files.html)**: 소스 코드 파일 브라우징

### 주요 시스템별 문서

#### 캐릭터 시스템
- \ref APlayerControl "플레이어 컨트롤러"
- \ref APlayerActor "플레이어 액터"
- \ref UFlySystem "비행 시스템"
- \ref UKnockbackSystem "넉백 시스템"
- \ref UHitStopSystem "히트스톱 시스템"

#### 네트워크 시스템
- \ref UHttpNetworkSystem "HTTP 통신 시스템"
- \ref UWebSocketSystem "WebSocket 통신 시스템"
- \ref FHttpMultipartFormData "멀티파트 폼 데이터"

#### UI 시스템
- \ref UDialogManager "다이얼로그 관리자"
- \ref UDialogWidget "다이얼로그 위젯"
- \ref ULoadingCircleManager "로딩 UI 관리자"

#### 데이터 관리
- \ref UGameDataManager "게임 데이터 관리자"
- \ref UCharacterData "캐릭터 데이터 에셋"
- \ref UVFXDataAsset "VFX 데이터 에셋"
- \ref USoundData "사운드 데이터 에셋"

#### 유틸리티
- \ref UEaseFunctionLibrary "Easing 함수 라이브러리"
- \ref UCommonFunctionLibrary "공용 함수 라이브러리"
- \ref UGameFunctionLibrary "게임 함수 라이브러리"

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

### 빌드 방법

```bash
# 저장소 클론
git clone https://github.com/doppleddiggong/Onepiece.git
cd Onepiece

# Git LFS 설정
git lfs install
git lfs pull

# Visual Studio 프로젝트 파일 생성
# Onepiece.uproject 우클릭 → "Generate Visual Studio project files"

# 빌드 (커맨드라인)
"<UE_ROOT>\Engine\Build\BatchFiles\Build.bat" OnepieceEditor Win64 Development -Project="<PROJECT_PATH>\Onepiece.uproject"
```

---

## 📅 개발 일정

| 단계 | 날짜 | 목표 |
|------|------|------|
| **기획** | 2025.11.18 | 프로젝트 구체화 및 핵심 기능 설계 |
| **프로토타입** | 2025.12.09 | 핵심 기능 최소 1개 이상 실체 구현 |
| **알파** | 2025.12.22 | 각 기능들의 실체 연동 |
| **베타** | 2026.01.05 | 기술적 완성도 평가 |
| **최종 발표** | 2026.01.08 | 기업 초청 평가 |

---

## 📞 관련 링크

- **GitHub Repository**: [doppleddiggong/Onepiece](https://github.com/doppleddiggong/Onepiece)
- **개발 문서 웹사이트**: [HonKit 문서](https://doppleddiggong.github.io/Onepiece/honkit/)
- **이슈 트래커**: [GitHub Issues](https://github.com/doppleddiggong/Onepiece/issues)

---

## 📄 라이선스

이 프로젝트는 MIT 라이선스를 따릅니다. 자세한 내용은 [LICENSE](https://github.com/doppleddiggong/Onepiece/blob/main/LICENSE) 파일을 참조하세요.

---

<div align="center">

**Made with ❤️ by KLingo Team**

*POTENUP Final Project (2025.11.10 ~ 2026.01.08)*

</div>
