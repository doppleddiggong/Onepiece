# Jira 연동 기능 테스트 가이드

## 사전 준비

### 1. Jira API 토큰 발급

1. https://id.atlassian.com/manage-profile/security/api-tokens 접속
2. "Create API token" 클릭
3. 토큰 이름 입력 (예: "Local Test")
4. 생성된 토큰을 복사 (한 번만 표시됨!)

### 2. Python 패키지 설치

```bash
pip install requests pyyaml openai
```

## 테스트 방법

### 방법 1: 테스트 스크립트 실행 (가장 쉬움)

#### Windows
```bash
cd e:\UE\Onepiece
.github\scripts\jira\test_jira.bat
```

#### Linux/Mac
```bash
cd /path/to/Onepiece
bash .github/scripts/jira/test_jira.sh
```

스크립트가 실행되면:
1. JIRA_API_TOKEN 입력 (발급받은 토큰)
2. OPENAI_API_KEY 입력 (선택사항, AI 분석을 원하면 입력)
3. 자동으로 테스트 실행

### 방법 2: 수동으로 환경변수 설정 후 실행

#### Windows PowerShell
```powershell
$env:JIRA_SITE="jeonminwoo0625-1762930390335"
$env:JIRA_EMAIL="ju100.bae@gmail.com"
$env:JIRA_PROJECT="UN"
$env:JIRA_API_TOKEN="your-api-token-here"
$env:OPENAI_API_KEY="your-openai-key-here"  # 선택사항

# 테스트용 DevLog 생성
$today = Get-Date -Format "yyyy-MM-dd"
New-Item -Force -Path "Documents\DevLog\Daily\$today.md" -Value @"
# Daily DevLog - Test

## 작업 내용
- Jira 연동 기능 테스트
"@

# 실행
python .github\scripts\jira\insert_progress.py
```

#### Linux/Mac Bash
```bash
export JIRA_SITE="jeonminwoo0625-1762930390335"
export JIRA_EMAIL="ju100.bae@gmail.com"
export JIRA_PROJECT="UN"
export JIRA_API_TOKEN="your-api-token-here"
export OPENAI_API_KEY="your-openai-key-here"  # 선택사항

# 테스트용 DevLog 생성
TODAY=$(date +%Y-%m-%d)
mkdir -p Documents/DevLog/Daily
cat > "Documents/DevLog/Daily/$TODAY.md" << EOF
# Daily DevLog - Test

## 작업 내용
- Jira 연동 기능 테스트
EOF

# 실행
python .github/scripts/jira/insert_progress.py
```

### 방법 3: 개별 스크립트 단계별 실행 (디버깅용)

```bash
# 1단계: Jira 이슈 가져오기
python scripts/jira/fetch_issues.py

# 결과 확인
cat Documents/DevLog/Metrics/jira_issues.json

# 2단계: 진척도 계산
python scripts/jira/calc_progress.py

# 결과 확인
cat Documents/DevLog/Metrics/jira_progress.json

# 3단계: DevLog에 삽입
python scripts/jira/insert_progress.py

# 결과 확인
cat Documents/DevLog/Daily/$(date +%Y-%m-%d).md
```

## 테스트 결과 확인

### 1. 생성된 파일 확인

다음 파일들이 생성되었는지 확인:

- `Documents/DevLog/Metrics/jira_issues.json` - Jira 이슈 목록
- `Documents/DevLog/Metrics/jira_progress.json` - 진척도 계산 결과
- `Documents/DevLog/Daily/YYYY-MM-DD.md` - Jira 섹션이 추가된 DevLog

### 2. DevLog 내용 확인

DevLog 파일을 열어서 다음 섹션이 추가되었는지 확인:

```markdown
## Jira 개발 진척도

### 전체 진행도
- Epic 진행률: X/Y (Z%)
- Task 진행률: X/Y (Z%)

### Epic 상세
- 🔄 [KEY] Epic 이름
  - Child Tasks: X/Y (Z%)
```

AI 분석을 활성화한 경우:
```markdown
## AI 개발 분석 및 피드백

...
```

### 3. 콘솔 출력 확인

정상 실행 시 다음과 같은 출력이 표시됩니다:

```
=== Starting Jira Progress Insertion ===

[1/5] Fetching Jira issues...
Fetching issues from Jira project: UN
Found 50 issues
✓ Issues saved to Documents/DevLog/Metrics/jira_issues.json

[2/5] Calculating progress...
Processing 50 issues...
✓ Progress calculated and saved to Documents/DevLog/Metrics/jira_progress.json

[3/5] Finding today's DevLog...

[4/5] Loading progress data...

[5/5] Inserting progress into DevLog...
  Generating AI analysis...
✓ Jira progress inserted into Documents/DevLog/Daily/2025-11-17.md
```

## GitHub Actions에서 테스트

### 1. GitHub Secrets 설정

Repository Settings → Secrets and variables → Actions에서:
- `JIRA_API_TOKEN` 추가

### 2. Workflow 수동 실행

1. GitHub 저장소 → Actions 탭
2. "Report Daily" workflow 선택
3. "Run workflow" 클릭
4. 파라미터 입력:
   - `date`: 비워두거나 오늘 날짜 (예: 2025-11-17)
   - `use_gpt`: true (GPT 피드백 포함)
5. "Run workflow" 실행

### 3. 실행 로그 확인

Workflow 실행 로그에서:
- "Load Jira Config" 단계에서 설정이 올바르게 로드되었는지 확인
- "Insert Jira Progress into DevLog" 단계의 출력 확인

## 문제 해결

### API 인증 실패
```
ERROR: Failed to fetch issues from Jira
  401 Unauthorized
```
→ JIRA_API_TOKEN이 올바른지 확인

### 이슈를 찾을 수 없음
```
Found 0 issues
```
→ JIRA_PROJECT 키가 올바른지 확인 (대소문자 구분)

### DevLog 파일이 없음
```
WARNING: DevLog file not found
```
→ 정상 동작 (DevLog가 없으면 스킵됨)
→ 테스트 시에는 수동으로 DevLog 파일 생성 필요

### OpenAI API 오류
```
WARNING: Failed to generate AI analysis
```
→ OPENAI_API_KEY 확인 또는 AI 분석 없이 진행

## 성공 기준

✅ jira_issues.json에 프로젝트 이슈 목록이 저장됨
✅ jira_progress.json에 진척도가 계산됨
✅ DevLog 파일에 "Jira 개발 진척도" 섹션이 추가됨
✅ Epic과 Task의 진행률이 올바르게 표시됨
✅ (선택) AI 분석 섹션이 생성됨

모든 항목이 체크되면 성공입니다! 🎉
