# 설정 파일 관리 가이드

## 📁 설정 파일 구조

프로젝트에는 3가지 종류의 설정 파일이 있습니다:

```
.github/
├── config.yml                              # 공개 설정 (Git 추적 ✅)
├── config.yml.template                     # 템플릿 (Git 추적 ✅)
├── config.env                              # 비공개 설정 (Git 추적 ❌)
└── scripts/
    └── devlog_klingo/
        ├── config.json                     # KLingo 설정 (Git 추적 ✅)
        └── config.env.template             # 템플릿 (Git 추적 ✅)
```

---

## 🔐 설정 파일별 역할

### 1. `config.yml` (공개 설정)

**용도**: GitHub Actions 워크플로우 중앙 설정
**Git 추적**: ✅ Yes (공개 데이터만)
**사용 위치**: GitHub Actions workflows

**포함 내용**:
- GPT 활성화 여부 (`gpt.daily_enabled`, `gpt.weekly_enabled`)
- Discord 알림 활성화 (`discord.enabled`)
- Confluence 설정 (`confluence.enabled`, `confluence.domain`)
- Jira 설정 (`jira.enabled`, `jira.site`, `jira.email`, `jira.project`)
- 디렉토리 경로 (`devlog.output_dir`)

**❌ 포함하면 안 되는 것**:
- API 키
- 비밀번호
- 토큰

**예시**:
```yaml
gpt:
  daily_enabled: true
  weekly_enabled: true

jira:
  enabled: true
  site: "jeonminwoo0625-1762930390335"  # 공개 가능
  email: "ju100.bae@gmail.com"         # 공개 가능
  project: "UN"                        # 공개 가능
  # token은 여기 없음! ❌
```

---

### 2. `config.env` (비공개 설정)

**용도**: 로컬 개발 환경의 비밀 정보
**Git 추적**: ❌ No (`.gitignore`로 차단)
**사용 위치**: 로컬 개발, 로컬 테스트

**포함 내용**:
- `OPENAI_API_KEY`: OpenAI API 키
- `JIRA_TOKEN`: Jira API 토큰
- `JIRA_SITE`, `JIRA_EMAIL`, `JIRA_PROJECT`: Jira 정보
- 기타 민감한 정보

**예시**:
```env
# Jira 설정
JIRA_SITE=jeonminwoo0625-1762930390335
JIRA_PROJECT=UN
JIRA_EMAIL=ju100.bae@gmail.com
JIRA_TOKEN=your-jira-api-token-here

# OpenAI API
OPENAI_API_KEY=sk-proj-your-openai-api-key-here
OPENAI_ASSISTANT_ID=asst_your-assistant-id-here
```

**⚠️ 주의**: 이 파일은 절대 Git에 커밋하지 마세요!

---

### 3. `config.env.template` (템플릿)

**용도**: 다른 개발자들이 참고할 템플릿
**Git 추적**: ✅ Yes
**사용 위치**: 새로운 개발 환경 설정 시 참고

**포함 내용**:
- 설정 항목 이름과 설명
- 실제 값은 비워두거나 예시 값

**예시**:
```env
# Jira 설정 파일
# 이 파일을 config.env로 복사하고 실제 값을 입력하세요

# Jira 사이트 도메인 (예: jeonminwoo0625-1762930390335)
JIRA_SITE=

# 프로젝트 키 (예: UN)
JIRA_PROJECT=

# Jira 계정 이메일
JIRA_EMAIL=

# Jira API 토큰 (https://id.atlassian.com/manage-profile/security/api-tokens)
JIRA_TOKEN=

# OpenAI API 키 (https://platform.openai.com/api-keys)
OPENAI_API_KEY=
```

---

### 4. `scripts/devlog_klingo/config.json` (KLingo 설정)

**용도**: KLingo DevLog 시스템의 구조 설정
**Git 추적**: ✅ Yes (민감한 정보는 환경 변수로)
**사용 위치**: 로컬 및 GitHub Actions

**포함 내용**:
- 프로젝트 경로 (`project_root`)
- CSV 파일 경로 (`developers_csv`)
- 회의록 디렉토리 (`meeting_dir`)
- Jira 룰 파일 (`jira_rules`)
- GPT 모델 이름 (`gpt_model`)

**❌ 포함하면 안 되는 것**:
- API 키 값 (비워두고 환경 변수로 대체)
- 토큰 값

**예시**:
```json
{
  "project_root": "../../..",
  "developers_csv": ".github/data/developers.csv",
  "meeting_dir": "Documents/Meeting/Daily",
  "jira_rules": ".github/data/jira_map_rules.yaml",
  "output_dir": "Documents/DevLog/Daily",
  "weekly_output_dir": "Documents/DevLog/Weekly",

  "use_gpt": true,
  "gpt_model": "gpt-4o",

  "jira": {
    "url": "",      // 환경 변수로 설정
    "email": "",    // 환경 변수로 설정
    "token": "",    // 환경 변수로 설정
    "project": ""   // 환경 변수로 설정
  },

  "openai_api_key": ""  // 환경 변수로 설정
}
```

---

## 🔄 설정 우선순위

설정 값은 다음 우선순위로 읽힙니다:

### 로컬 개발 환경

```
1. 환경 변수 (export OPENAI_API_KEY=...)
2. config.env (KLingo 스크립트가 자동 로드)
3. config.json
4. 기본값
```

### GitHub Actions

```
1. GitHub Secrets (예: secrets.OPENAI_API_KEY)
2. config.yml (workflow에서 읽음)
3. config_runtime.json (workflow에서 동적 생성)
4. 기본값
```

---

## 📊 설정 데이터 분류

### 공개 데이터 (Git 커밋 가능)

- ✅ GPT 활성화 여부
- ✅ Discord 활성화 여부
- ✅ Jira 사이트명 (`jeonminwoo0625-1762930390335`)
- ✅ Jira 이메일 (`ju100.bae@gmail.com`)
- ✅ Jira 프로젝트 키 (`UN`)
- ✅ 디렉토리 경로
- ✅ 파일 경로

**위치**: `config.yml`, `config.json`

### 비공개 데이터 (Git 커밋 금지)

- 🔒 `OPENAI_API_KEY`
- 🔒 `JIRA_TOKEN`
- 🔒 Discord Webhook URL
- 🔒 Confluence API 토큰

**로컬**: `config.env` (`.gitignore`로 차단)
**GitHub Actions**: GitHub Secrets

---

## 🛠️ 실전 사용법

### 시나리오 1: 새 개발자가 프로젝트 시작

1. **저장소 클론**
   ```bash
   git clone https://github.com/doppleddiggong/Onepiece.git
   cd Onepiece
   ```

2. **config.env 생성**
   ```bash
   # 템플릿 복사
   cp .github/config.env.template .github/config.env

   # 또는 devlog_klingo 템플릿 사용
   cp .github/scripts/devlog_klingo/config.env.template .github/config.env
   ```

3. **비공개 값 입력**
   ```bash
   # .github/config.env 파일 편집
   # OPENAI_API_KEY, JIRA_TOKEN 등 실제 값 입력
   ```

4. **로컬 테스트**
   ```bash
   cd .github/scripts/devlog_klingo
   python devlog_generate.py --date 2025-11-21 --branch dopple
   ```

### 시나리오 2: GitHub Actions 설정

1. **공개 설정 확인**
   - `.github/config.yml` 파일 확인
   - GPT, Jira 활성화 여부 설정

2. **GitHub Secrets 설정**
   - `Settings` > `Secrets and variables` > `Actions`
   - 다음 Secrets 추가:
     - `OPENAI_API_KEY`
     - `JIRA_API_TOKEN`
     - `DISCORD_WEBHOOK_DEVLOG`

3. **Workflow 실행**
   - `Actions` 탭에서 수동 실행 또는 자동 스케줄

### 시나리오 3: 설정 값 업데이트

#### 공개 설정 변경 (예: GPT 비활성화)

```bash
# .github/config.yml 편집
vim .github/config.yml

# gpt.daily_enabled: false로 변경

# Git 커밋
git add .github/config.yml
git commit -m "config: disable daily GPT analysis"
git push
```

#### 비공개 설정 변경 (예: API 키 갱신)

**로컬**:
```bash
# .github/config.env 편집
vim .github/config.env

# OPENAI_API_KEY 값 업데이트
# Git 커밋 안 함!
```

**GitHub Actions**:
1. `Settings` > `Secrets and variables` > `Actions`
2. `OPENAI_API_KEY` 수정
3. 새 값 입력

---

## 🐛 문제 해결

### "config.env를 찾을 수 없습니다"

**원인**: 로컬에 `config.env` 파일이 없음

**해결**:
```bash
cp .github/config.env.template .github/config.env
# 실제 값 입력
```

### "OPENAI_API_KEY가 설정되지 않았습니다"

**로컬**:
- `.github/config.env`에 값이 있는지 확인
- 환경 변수로 직접 설정: `export OPENAI_API_KEY=...`

**GitHub Actions**:
- GitHub Secrets에 `OPENAI_API_KEY` 추가되어 있는지 확인

### "config.env가 Git에 커밋되었습니다"

**긴급 조치**:
```bash
# Git 히스토리에서 완전히 제거
git rm --cached .github/config.env
git commit -m "chore: remove sensitive config.env from git"

# .gitignore 확인
grep "*.env" .gitignore  # 있어야 함

# API 키 즉시 재발급
# - OpenAI 키: https://platform.openai.com/api-keys
# - Jira 토큰: https://id.atlassian.com/manage-profile/security/api-tokens
```

---

## 📋 체크리스트

### 로컬 개발 환경 설정

- [ ] `.github/config.env` 파일 생성
- [ ] `OPENAI_API_KEY` 값 입력
- [ ] `JIRA_TOKEN` 값 입력
- [ ] `.gitignore`에 `*.env` 있는지 확인
- [ ] `git status`로 `config.env`가 추적 안 되는지 확인

### GitHub Actions 설정

- [ ] `.github/config.yml` 파일 커밋됨
- [ ] GitHub Secrets에 `OPENAI_API_KEY` 추가
- [ ] GitHub Secrets에 `JIRA_API_TOKEN` 추가
- [ ] GitHub Secrets에 `DISCORD_WEBHOOK_DEVLOG` 추가 (선택)
- [ ] Workflow 수동 실행으로 테스트

---

## 📞 추가 도움

- **설정 템플릿**: `.github/config.env.template` 참조
- **Secrets 가이드**: `.github/workflows/GITHUB_SECRETS_SETUP.md` 참조
- **마이그레이션**: `.github/workflows/KLINGO_MIGRATION_GUIDE.md` 참조

---

**작성일**: 2025-11-23
**버전**: 1.0.0
