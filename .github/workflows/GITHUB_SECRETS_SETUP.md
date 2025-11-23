# GitHub Secrets 설정 가이드

## 🔑 필수 Secrets 설정

KLingo DevLog 시스템이 제대로 작동하려면 다음 GitHub Secrets를 설정해야 합니다.

---

## 1. OPENAI_API_KEY (필수)

GPT 기반 DevLog 분석에 사용됩니다.

### 설정 방법

1. **GitHub 저장소** 페이지로 이동
2. **Settings** 탭 클릭
3. 왼쪽 메뉴에서 **Secrets and variables** > **Actions** 클릭
4. **New repository secret** 버튼 클릭
5. 다음 정보 입력:
   - **Name**: `OPENAI_API_KEY`
   - **Secret**: 로컬 `.github/config.env` 파일의 `OPENAI_API_KEY` 값 복사

### 로컬 값 확인

```bash
# 로컬에서 API 키 확인
grep OPENAI_API_KEY .github/config.env
```

**현재 로컬 값 확인하기**:
```bash
grep OPENAI_API_KEY .github/config.env
```

출력 예시:
```
OPENAI_API_KEY=sk-proj-xxxxxxxxxxxxxxxxxxxxx
```

⚠️ **주의**: 로컬에서 확인한 실제 값을 GitHub Secrets에 복사하세요. (앞의 `OPENAI_API_KEY=` 부분은 제외)

---

## 2. JIRA_API_TOKEN (선택사항)

Jira 이슈 연동에 사용됩니다.

### 설정 방법

1. 위와 동일한 방법으로 **New repository secret** 클릭
2. 다음 정보 입력:
   - **Name**: `JIRA_API_TOKEN`
   - **Secret**: 로컬 `.github/config.env` 파일의 `JIRA_TOKEN` 값 복사

### 로컬 값 확인

```bash
# 로컬에서 Jira 토큰 확인
grep JIRA_TOKEN .github/config.env
```

**현재 로컬 값 확인하기**:
```bash
grep JIRA_TOKEN .github/config.env
```

출력 예시:
```
JIRA_TOKEN=ATATTxxxxxxxxxxxxxxxxxx
```

---

## 3. DISCORD_WEBHOOK_DEVLOG (선택사항)

Discord 알림 전송에 사용됩니다.

### Discord Webhook URL 생성

1. Discord 서버에서 **서버 설정** > **연동** > **Webhook** 클릭
2. **새 Webhook** 생성
3. Webhook URL 복사

### GitHub Secrets 설정

1. **New repository secret** 클릭
2. 다음 정보 입력:
   - **Name**: `DISCORD_WEBHOOK_DEVLOG`
   - **Secret**: 복사한 Discord Webhook URL

---

## 📋 설정 확인 체크리스트

설정이 완료되면 다음을 확인하세요:

- [ ] **OPENAI_API_KEY** Secret 추가 완료
- [ ] **JIRA_API_TOKEN** Secret 추가 완료 (선택)
- [ ] **DISCORD_WEBHOOK_DEVLOG** Secret 추가 완료 (선택)

---

## 🧪 설정 테스트

### 방법 1: 수동 Workflow 실행

1. **Actions** 탭으로 이동
2. 왼쪽에서 **KLingo Daily DevLog** 선택
3. **Run workflow** 버튼 클릭
4. 다음 입력:
   - **date**: 어제 날짜 (예: 2025-11-22)
   - **branch**: main
   - **use_gpt**: true (체크)
5. **Run workflow** 실행

### 방법 2: 로그 확인

Workflow 실행 후 로그에서 다음을 확인:

**성공 시**:
```
Checking environment variables...
OK: OPENAI_API_KEY is set
Runtime config created: use_gpt=True, has_api_key=True
```

**실패 시**:
```
Checking environment variables...
WARNING: OPENAI_API_KEY is not set. GPT analysis will be disabled.
Runtime config created: use_gpt=False, has_api_key=False
```

---

## 🐛 문제 해결

### "OPENAI_API_KEY is not set" 경고

**원인**: GitHub Secrets에 `OPENAI_API_KEY`가 없거나 이름이 틀림

**해결**:
1. GitHub 저장소 **Settings** > **Secrets and variables** > **Actions** 확인
2. Secret 이름이 정확히 `OPENAI_API_KEY`인지 확인 (대소문자 구분)
3. Secret 값이 비어있지 않은지 확인

### "use_gpt=False" 로그

**원인 1**: OPENAI_API_KEY Secret이 없음
- 위의 설정 방법대로 Secret 추가

**원인 2**: `.github/config.yml`에서 `gpt.daily_enabled: false`
- `config.yml` 파일 확인 및 수정:
  ```yaml
  gpt:
    daily_enabled: true  # false → true로 변경
  ```

**원인 3**: Workflow 실행 시 `use_gpt: false` 입력
- 수동 실행 시 `use_gpt` 체크박스를 **true**로 설정

---

## 📸 스크린샷 예시

### GitHub Secrets 페이지
```
Settings > Secrets and variables > Actions

Repository secrets:
┌─────────────────────────┬──────────────┐
│ OPENAI_API_KEY          │ Updated ...  │
│ JIRA_API_TOKEN          │ Updated ...  │
│ DISCORD_WEBHOOK_DEVLOG  │ Updated ...  │
└─────────────────────────┴──────────────┘
```

---

## 📞 추가 도움

설정 후에도 문제가 발생하면:

1. **Workflow 로그 전체 확인**: Actions 탭에서 실행된 workflow의 로그 확인
2. **config.yml 확인**: `.github/config.yml` 파일의 설정 확인
3. **로컬 테스트**: 로컬에서 스크립트 실행 후 결과 비교

---

**작성일**: 2025-11-23
**버전**: 1.0.0
