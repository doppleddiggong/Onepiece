# Jira Progress Tracking for DevLog

이 디렉토리는 Jira API를 활용하여 개발 진행상황을 일일 개발일지에 자동으로 삽입하는 스크립트들을 포함합니다.

## 파일 구조

```
.github/scripts/jira/
├── fetch_issues.py       # Jira API로 이슈 목록 가져오기
├── calc_progress.py      # Epic/Task별 진척도 계산
├── insert_progress.py    # DevLog에 진척도 섹션 삽입 (메인 스크립트)
├── test_jira.bat         # Windows 로컬 테스트 스크립트
└── README.md
```

## 설정 방법

### 1. GitHub Secrets 설정

다음 secrets를 GitHub 저장소에 추가해야 합니다:

- `JIRA_SITE`: Jira 사이트 도메인 (예: `jeonminwoo0625-1762930390335`)
- `JIRA_EMAIL`: Jira 계정 이메일
- `JIRA_PROJECT`: Jira 프로젝트 키 (예: `UN`)
- `JIRA_API_TOKEN`: Jira API 토큰
- `OPENAI_API_KEY`: OpenAI API 키 (AI 분석용, 선택사항)

#### Jira API 토큰 발급 방법

1. https://id.atlassian.com/manage-profile/security/api-tokens 접속
2. "Create API token" 클릭
3. 토큰 이름 입력 (예: "GitHub Actions")
4. 생성된 토큰을 복사하여 GitHub Secrets에 저장

### 2. config.yml 설정

`.github/config.yml` 파일에서 Jira 연동을 활성화합니다:

```yaml
jira:
  enabled: true
  site: "jeonminwoo0625-1762930390335"
  email: "ju100.bae@gmail.com"
  project: "UN"
```

### 3. GitHub Actions 자동 실행

Daily DevLog workflow (`daily-report.yml`)가 실행될 때 자동으로 Jira 진척도가 삽입됩니다.

## 수동 실행

### 전체 프로세스 실행

```bash
# 환경 변수 설정
export JIRA_SITE="jeonminwoo0625-1762930390335"
export JIRA_EMAIL="ju100.bae@gmail.com"
export JIRA_PROJECT="UN"
export JIRA_API_TOKEN="your-api-token"
export OPENAI_API_KEY="your-openai-key"  # 선택사항

# 전체 프로세스 실행
python .github/scripts/jira/insert_progress.py
```

### 개별 스크립트 실행

```bash
# 1. Jira 이슈 가져오기
python .github/scripts/jira/fetch_issues.py

# 2. 진척도 계산
python .github/scripts/jira/calc_progress.py

# 3. DevLog에 삽입 (위 두 단계는 자동 실행됨)
python .github/scripts/jira/insert_progress.py
```

## 출력 결과

### 생성되는 파일

- `Documents/DevLog/Metrics/jira_issues.json`: Jira 이슈 목록
- `Documents/DevLog/Metrics/jira_progress.json`: 진척도 계산 결과
- `Documents/DevLog/Daily/YYYY-MM-DD.md`: Jira 진척도가 추가된 DevLog

### DevLog에 추가되는 섹션

1. **Jira 개발 진척도**
   - 전체 진행도 (Epic/Task 진행률)
   - Epic 상세 정보 (하위 Task 진행률)

2. **AI 개발 분석 및 피드백** (OPENAI_API_KEY가 설정된 경우)
   - 전체 개발 진행 요약
   - 주의가 필요한 Epic/Task
   - 우선순위 추천
   - 개발 프로세스 개선 제안

## 예시 출력

```markdown
## Jira 개발 진척도

### 전체 진행도
- Epic 진행률: 3/12 (25.0%)
- Task 진행률: 41/105 (39.0%)

### Epic 상세
- 🔄 [UN-123] Step1 Read
  - Child Tasks: 6/12 (50.0%)
- 🔄 [UN-456] Tutor System
  - Child Tasks: 1/8 (12.5%)
- ✅ [UN-789] Initial Setup
  - Child Tasks: 5/5 (100.0%)

## AI 개발 분석 및 피드백

### 전체 개발 진행 요약
현재 프로젝트는 전체 Epic의 25%가 완료되었으며, Task 단위로는 39%의 진행률을 보이고 있습니다...
```

## 문제 해결

### Jira API 인증 실패

- JIRA_API_TOKEN이 올바른지 확인
- JIRA_EMAIL이 정확한지 확인
- API 토큰이 만료되지 않았는지 확인

### DevLog 파일을 찾을 수 없음

- DevLog가 먼저 생성되어야 Jira 진척도를 삽입할 수 있습니다
- `build_daily_log.py`가 먼저 실행되는지 확인

### AI 분석이 생성되지 않음

- OPENAI_API_KEY가 설정되어 있는지 확인
- `openai` 패키지가 설치되어 있는지 확인: `pip install openai`

## 비용 및 제한사항

### Jira API

- 무료 플랜: 시간당 API 요청 제한 있음
- 하루 1회 실행 권장

### OpenAI API

- GPT-4o-mini 모델 사용 (저비용)
- 1회 실행당 약 $0.001~0.005 예상
- 비용이 부담되면 config.yml에서 AI 분석 비활성화 가능

## 라이선스

이 프로젝트의 라이선스를 따릅니다.
