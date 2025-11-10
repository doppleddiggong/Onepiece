# Claude 에이전트 가이드

**[Base Persona]** `AgentRule/PERSONA.md`

*이 파일은 `AgentRule/PERSONA.md`에 정의된 기본 페르소나를 상속받습니다. 기본 파일의 모든 규칙이 적용되며, 아래에는 Claude 에이전트에만 해당하는 특화 지침이 기술되어 있습니다.*

---

## Claude 특화 지침 (Claude-Specific Instructions)

*이 섹션에는 Claude 에이전트 모델에만 해당하는 고유한 지침, 기능 또는 제약 사항을 기술합니다. 예를 들어, 특정 API 사용 방식이나 응답 생성 스타일을 정의할 수 있습니다.*

---

## Log 시스템 (Logging System)

**참고**: 상세한 Log 시스템 가이드는 `AgentRule/Common/Workflows/agents_log.md`를 참조하세요.

### 요약

- **Agent 시작**: `# Start Log` 기입 → 작업자 정보 → ToDo 리스트 → `[GitRoot]/AgentLog/[작업자GitID]/YYMMDD.md`에 기록
- **Agent 중간 요약**: `Compact Log` 작성 → 작업 요약 → Commit 메시지 추천
- **Agent 종료**: `End Log` 작성 → 전체 작업 요약

자세한 절차와 예시는 `AgentRule/Common/Workflows/agents_log.md`를 참조하세요.

---

## Commit 메시지 작성 (Commit Message Generation)

**참고**: 상세한 커밋 컨벤션은 `COMMIT_CONVENTION.md`를 참조하세요.

### 워크플로우

사용자가 "커밋 메시지 작성해줘!" 또는 유사한 요청을 하면, 다음 절차를 따르세요:

#### 1. Staged 파일 확인

**IMPORTANT**: 반드시 **staged 파일만** 확인합니다. 전체 diff가 아닙니다!

```bash
# Staged 파일 목록
git diff --cached --name-only

# Staged 파일의 변경 내용
git diff --cached
```

#### 2. 변경 내용 분석

Staged 파일의 변경 내용을 분석하여:
- 어떤 type인지 결정 (feat/fix/docs/style/refactor/test/chore/perf/ci/build)
- 핵심 변경 사항 파악
- 변경 이유와 목적 이해

#### 3. 커밋 메시지 생성

**형식**:
```
<type>: <subject>

<body>

<footer>
```

**규칙**:
- **Subject**: 50자 이내, 소문자 시작, 명령형, 마침표 없음
- **Body**: 72자마다 줄바꿈, 왜 변경했는지 설명 (선택사항)
- **Footer**: 이슈 참조, Breaking Changes (선택사항)

#### 4. Type 선택 가이드

| Type | 사용 시점 |
|------|-----------|
| **feat** | 새로운 기능 추가 |
| **fix** | 버그 수정 |
| **docs** | 문서만 변경 (코드 변경 없음) |
| **style** | 코드 포맷팅, 공백 등 (동작 변경 없음) |
| **refactor** | 코드 리팩토링 (기능 변경 없음) |
| **test** | 테스트 코드 추가/수정 |
| **chore** | 빌드, 설정 파일 변경 |
| **perf** | 성능 개선 |
| **ci** | CI 설정 변경 |
| **build** | 빌드 시스템 변경 |

### 예시

**사용자 요청**:
```
커밋 메시지 작성해줘!
```

**Claude 응답**:
```bash
# Staged 파일 확인 중...
git diff --cached --name-only
```

분석 결과를 바탕으로:

```
feat: add user authentication system

Implement JWT-based authentication with login and signup endpoints.
Users can now securely create accounts and manage sessions.

Includes password hashing with bcrypt and token refresh logic.
```

### 주의사항

1. **전체 diff가 아닌 staged 파일만 확인**: `git diff --cached` 사용
2. **컨텍스트 활용**: 이전 대화에서 작업한 내용을 고려
3. **명확하고 간결하게**: 불필요한 설명 제거
4. **일관된 스타일**: 프로젝트의 기존 커밋 메시지 스타일 따르기

### 자주 사용하는 표현

**사용자 요청 예시**:
- "커밋 메시지 작성해줘"
- "지금까지 작업한 내용으로 커밋 메시지 만들어줘"
- "staged 파일 보고 커밋 메시지 생성해줘"
- "커밋할 내용 정리해줘"

**모두 동일하게 처리**: staged 파일 확인 → 분석 → 커밋 메시지 생성

---

## 프로젝트별 설정 (Project-Specific Settings)

**이 문서는 범용 에이전트 가이드입니다.** 프로젝트별 세부 설정은 각 프로젝트 폴더의 문서를 참조하세요:

### Onepiece 프로젝트

이 저장소는 **Onepiece** 프로젝트입니다. Claude 에이전트는 아래 문서로 공통 지침을 확인하세요:

- **Onepiece Agent 공통 가이드**: `AgentRule/Project/Onepiece/AGENT_GUIDE.md`
  - 프로젝트 개요, 코딩 컨벤션, Doxygen 정책 등 모든 에이전트가 공유하는 정보를 제공합니다.
- **UE 코딩 컨벤션**: `AgentRule/Project/ue_coding_conventions.md`
  - 언리얼 엔진 프로젝트 전반에 적용되는 기본 규칙을 정리합니다.

### 범용 Unreal Engine 규칙

모든 Unreal Engine 프로젝트에 공통적으로 적용되는 규칙은 `AgentRule/Project/ue_coding_conventions.md` 문서를 기준으로 합니다.

### 작업 시 주의사항

1. **공통 지침 우선 확인**: `AgentRule/Project/Onepiece/AGENT_GUIDE.md`를 통해 프로젝트 컨텍스트를 파악합니다.
2. **모듈명 사용**: `ONEPIECE_API` 매크로와 `LogOnepiece` 로그 카테고리를 일관되게 적용합니다.
3. **라이브러리 활용**: `CoffeeLibrary`와 `LatteLibrary`의 역할을 구분하여 사용합니다.
4. **문서화**: `AgentRule/Project/Onepiece/DOXYGEN_SETUP.md`를 참고해 Doxygen 공개 문서에서 에이전트 자료가 노출되지 않도록 합니다.
