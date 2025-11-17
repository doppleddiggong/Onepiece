# Confluence Auto Upload

자동으로 Markdown 파일을 Confluence에 업로드하는 GitHub Actions 시스템입니다.

## 작동 방식

다음 폴더의 `.md` 파일이 `main` 또는 `master` 브랜치에 push되면 자동으로 Confluence에 업로드됩니다:

- `Documents/Meeting/Daily/*.md` → Confluence Daily 페이지
- `Documents/Meeting/Common/*.md` → Confluence Common 페이지
- `Documents/DevLog/Daily/*.md` → Confluence UE 페이지
- `Documents/DevLog/Weekly/*.md` → Confluence UE 페이지

## 설정

### 1. GitHub Secrets 설정

GitHub 저장소의 Settings > Secrets and variables > Actions에서 다음 Secret을 추가해야 합니다:

- `JIRA_API_TOKEN`: Confluence API 토큰 (Jira와 동일한 토큰 사용)

### 2. Config.yml 설정

[.github/config.yml](./../config.yml) 파일에서 Confluence 설정을 확인/수정할 수 있습니다:

```yaml
confluence:
  enabled: true  # false로 설정하면 자동 업로드 비활성화
  domain: "your-domain.atlassian.net"
  email: "your-email@example.com"
  space_key: "YOUR_SPACE"
  parent_pages:
    daily: "2916376"   # Meeting Daily 페이지 ID
    common: "1900600"  # Meeting Common 페이지 ID
    ue: "1835026"      # UE DevLog 페이지 ID
```

## 파일 구조

```
.github/scripts/confluence/
├── api.py                    # Confluence REST API 래퍼
├── converter.py              # Markdown → HTML 변환기
├── utils.py                  # 유틸리티 함수
├── upload_to_confluence.py   # 메인 업로드 스크립트
├── requirements.txt          # Python 의존성
└── README.md                # 이 문서
```

## 자동 업로드 규칙

1. **파일명 → 페이지 제목 변환**
   - `251117_meeting.md` → `2025-11-17 — meeting`
   - `Meeting_251117_Daily.md` → `2025-11-17 — Meeting Daily`

2. **중복 처리**
   - 같은 제목의 페이지가 이미 존재하면 업데이트
   - 새 페이지인 경우 생성

3. **YAML Front Matter**
   - Markdown의 YAML front matter는 자동으로 제거됩니다

## 수동 실행

GitHub Actions 탭에서 "Confluence Auto Upload" 워크플로우를 선택하고 "Run workflow"를 클릭하여 특정 파일을 수동으로 업로드할 수 있습니다.

## 로컬 테스트

로컬에서 테스트하려면:

```bash
# 의존성 설치
pip install -r .github/scripts/confluence/requirements.txt

# 환경 변수 설정
export JIRA_API_TOKEN="your-api-token"

# 스크립트 실행
python .github/scripts/confluence/upload_to_confluence.py Documents/Meeting/Daily/251117_test.md
```

## 문제 해결

### 업로드가 실행되지 않는 경우

1. `.github/config.yml`에서 `confluence.enabled: true`인지 확인
2. GitHub Secrets에 `JIRA_API_TOKEN`이 등록되어 있는지 확인
3. 파일이 올바른 경로에 있는지 확인 (위의 4개 폴더 중 하나)
4. `.md` 확장자를 가진 파일인지 확인

### API 에러가 발생하는 경우

1. API 토큰이 유효한지 확인
2. Confluence 도메인, 이메일, 스페이스 키가 정확한지 확인
3. Parent 페이지 ID가 존재하는지 확인

## 지원되는 Markdown 기능

- 제목 (Headers)
- 목록 (Lists)
- 코드 블록 (Code Blocks)
- 테이블 (Tables)
- 링크 (Links)
- 이미지 (Images)
- 굵게/기울임 (Bold/Italic)

## 참고

- Confluence API 문서: https://developer.atlassian.com/cloud/confluence/rest/v1/intro/
- Python Markdown 라이브러리: https://python-markdown.github.io/
