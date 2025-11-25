# KLingo DevLog 자동 생성 시스템

GPT 기반 지능형 Daily/Weekly DevLog 자동 생성 시스템입니다.

## 특징

- **6가지 데이터 소스 통합**
  - Git diff (커밋 타임스탬프 기준)
  - 자동 생성된 커밋 메시지
  - 회의록 (Markdown + frontmatter)
  - 개발자 정보 (CSV)
  - Jira 매핑 룰 (YAML)
  - Jira API (작업자 기반 이슈 조회)

- **GPT 기반 분석**
  - 주요 변경 사항 자동 요약
  - 시스템 영향도 분석
  - 리스크 및 문제점 식별
  - 검증 필요 항목 제안
  - 회의록 기반 후속 작업 추적
  - Jira 이슈 자동 추론

- **KST 기준 날짜 계산**
  - UTC 00:00 = KST 09:00
  - 정확한 하루 단위 DevLog 생성

## 설치

### 1. Python 의존성 설치

```bash
cd .github/scripts/devlog_klingo
pip install -r requirements.txt
```

또는 UV 환경에서:

```bash
uv pip install -r requirements.txt
```

### 2. 환경 변수 설정

**Windows (PowerShell)**:
```powershell
$env:OPENAI_API_KEY="your-openai-api-key"
$env:JIRA_URL="https://your-domain.atlassian.net"
$env:JIRA_EMAIL="your-email@example.com"
$env:JIRA_TOKEN="your-jira-api-token"
```

**Linux/Mac (Bash)**:
```bash
export OPENAI_API_KEY="your-openai-api-key"
export JIRA_URL="https://your-domain.atlassian.net"
export JIRA_EMAIL="your-email@example.com"
export JIRA_TOKEN="your-jira-api-token"
```

또는 `config.json` 파일에 직접 설정할 수 있습니다 (권장하지 않음 - 보안상 환경 변수 사용 권장).

### 3. 설정 파일 확인

`.github/scripts/devlog_klingo/config.json`:
```json
{
  "project_root": "../../..",
  "developers_csv": ".github/data/developers.csv",
  "meeting_dir": "Documents/Meeting/Daily",
  "jira_rules": ".github/data/jira_map_rules.yaml",
  "output_dir": "Documents/DevLog/Daily",
  "use_gpt": true,
  "gpt_model": "gpt-4o"
}
```

## 사용법

### Daily DevLog 생성

#### 기본 실행 (어제 날짜, 현재 브랜치)

```bash
cd .github/scripts/devlog_klingo
python devlog_generate.py
```

#### 특정 날짜 지정

```bash
python devlog_generate.py --date 2025-11-20
```

#### 특정 브랜치 지정

```bash
python devlog_generate.py --branch dopple
```

#### 설정 파일 지정

```bash
python devlog_generate.py --config custom_config.json
```

#### 전체 옵션

```bash
python devlog_generate.py --help
```

### Weekly DevLog 생성

#### 이번 주 DevLog 생성

```bash
cd .github/scripts/devlog_klingo
python weekly_devlog_generate.py
```

#### 특정 날짜가 속한 주의 DevLog 생성

```bash
python weekly_devlog_generate.py --date 2025-11-20
```

#### 특정 주 번호로 생성

```bash
python weekly_devlog_generate.py --week 2025-W47
```

#### 전체 옵션

```bash
python weekly_devlog_generate.py --help
```

## 출력 형식

생성되는 DevLog는 다음 형식을 따릅니다:

```markdown
# Daily DevLog — YYYY-MM-DD

**개발자**: 배주백
**브랜치**: dopple
**커밋 수**: 5개
**변경 파일**: 12개

---

## 1. 오늘의 주요 변경
- 변경 사항 1
- 변경 사항 2
...

## 2. 시스템 영향도
- Network 시스템: ...
- Voice 시스템: ...

## 3. 리스크 및 문제
- 잠재적 문제점...

## 4. 검증 필요 항목
- 테스트 항목...

## 5. 회의 기반 후속 작업
- Action Items 연계...

## 6. 관련 Jira 이슈 (자동 추론)
- [ONE-123] ...

---

*생성 시간: YYYY-MM-DD HH:MM:SS*
```

## 파일 구조

```
.github/
  scripts/
    devlog_klingo/
      devlog_generate.py          # Daily DevLog 생성 스크립트
      weekly_devlog_generate.py   # Weekly DevLog 생성 스크립트
      weekly_stats.py             # 주간 통계 분석 모듈
      utils.py                    # 유틸리티 (날짜 계산 등)
      meeting_loader.py           # 회의록 로더
      diff_loader.py              # Git diff 로더
      jira_mapper.py              # Jira 매핑
      gpt_client.py               # GPT 클라이언트
      config.json                 # 설정 파일
      requirements.txt            # Python 의존성
      README.md                   # 이 파일
      templates/                  # 템플릿 디렉토리
        devlog_template.md             # Daily DevLog 템플릿
        devlog_prompt.txt              # Daily DevLog GPT 프롬프트
        weekly_devlog_template.md      # Weekly DevLog 템플릿
        weekly_devlog_prompt.txt       # Weekly DevLog GPT 프롬프트
  data/
    developers.csv              # 개발자 정보
    jira_map_rules.yaml         # Jira 매핑 룰
Documents/
  Meeting/
    Daily/
      Meeting_YYMMDD_Daily.md   # 회의록
  DevLog/
    Daily/
      YYYY-MM-DD.md             # 생성된 Daily DevLog
      YYYY-MM-DD.metrics.json   # Daily 메트릭 데이터
    Weekly/
      YYYY-WXX-Summary.md           # 생성된 Weekly DevLog
      YYYY-WXX-Summary.metrics.json # Weekly 메트릭 데이터
```

## 개별 모듈 테스트

각 모듈은 독립적으로 테스트 가능합니다:

```bash
# 날짜 계산 테스트
python utils.py

# 회의록 로더 테스트
python meeting_loader.py

# Git diff 로더 테스트
python diff_loader.py

# Jira 매퍼 테스트
python jira_mapper.py

# GPT 클라이언트 테스트
python gpt_client.py

# 주간 통계 모듈 테스트
python weekly_stats.py
```

## 트러블슈팅

### 1. "OpenAI API 키가 설정되지 않았습니다"

환경 변수 `OPENAI_API_KEY`를 설정하거나, `config.json`에서 `use_gpt: false`로 설정하여 Fallback 모드 사용.

### 2. "회의록을 찾을 수 없습니다"

회의록 파일명 형식이 `Meeting_YYMMDD_Daily.md`인지 확인.
날짜 형식이 맞지 않으면 `meeting_loader.py`에서 패턴 수정.

### 3. "Jira API 호출 실패"

- JIRA_URL, JIRA_EMAIL, JIRA_TOKEN 환경 변수 확인
- Jira API 토큰이 유효한지 확인
- 네트워크 연결 상태 확인

### 4. "커밋이 없습니다"

- 지정된 날짜 범위에 커밋이 있는지 확인
- Git 저장소 경로가 올바른지 확인
- KST 날짜 계산이 정확한지 확인

### 5. "GPT API Rate Limit 초과" 또는 "요청 실패"

**최적화 버전(v2.0)에서 해결됨:**

- 프롬프트 크기 50% 축소 (diff: 5000→2000자, 회의록: 1000→500자, 파일: 20→10개)
- 자동 재시도 로직 (최대 3회, exponential backoff: 2초, 4초, 8초)
- 토큰 수 추정 및 경고 메시지
- Rate limit 및 timeout 에러 자동 처리

만약 여전히 실패한다면:
1. `config.json`에서 `gpt_model`을 `gpt-4o-mini`로 변경
2. OpenAI 계정의 Rate Limit 상태 확인
3. API 키가 유효한지 확인

## GPT API 최적화 (v2.0)

### 최적화 내용

**프롬프트 크기 축소 (~50% 감소)**:
- Git diff: 5,000자 → 2,000자
- 회의록 내용: 1,000자 → 500자
- 변경 파일 목록: 20개 → 10개

**재시도 로직 추가**:
- 최대 3회 재시도 (Exponential Backoff)
- 대기 시간: 2초 → 4초 → 8초
- Rate limit (429) 및 timeout 에러 자동 감지

**토큰 추정 및 모니터링**:
- 요청 전 토큰 수 자동 추정 (~3자/토큰)
- 8,000 토큰 초과 시 경고 메시지
- 상세한 로그 출력

### 최적화 효과

- **토큰 사용량**: ~60% 감소
- **API 비용**: ~60% 절감
- **안정성**: Rate limit 에러 자동 복구
- **응답 시간**: 평균 20% 단축

### 설정 옵션

`gpt_client.py`의 `generate_devlog_with_gpt()` 함수:

```python
generate_devlog_with_gpt(
    data,
    api_key="your-key",
    model="gpt-4o",      # 또는 "gpt-4o-mini" (더 저렴)
    max_retries=3        # 재시도 횟수 (기본: 3)
)
```

## 개발자 정보 관리

`.github/data/developers.csv`:

```csv
branch,developer,jira_account
dopple,배주백,jubaeck Bae
cubee,이은희,이은희
CookieAndJuice,이치훈,이치훈
```

새로운 개발자를 추가하려면 이 파일에 행을 추가하세요.

## Jira 매핑 룰 관리

`.github/data/jira_map_rules.yaml`:

파일 경로 패턴을 시스템명으로 매핑합니다.
새로운 시스템을 추가하거나 기존 매핑을 수정할 수 있습니다.

```yaml
Network:
  - "Source/Onepiece/Network/"
Voice:
  - "Source/Onepiece/Voice/"
```

## 라이선스

이 프로젝트는 Onepiece 프로젝트의 일부입니다.

## 문의

문제가 발생하면 팀 DevOps 담당자에게 문의하세요.
