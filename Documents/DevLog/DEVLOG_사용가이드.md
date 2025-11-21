# KLingo DevLog 자동 생성 시스템 사용 가이드

## 📌 개요

Git 커밋, 회의록, Jira를 통합하여 GPT 기반으로 Daily DevLog를 자동 생성하는 시스템입니다.

---

## 🚀 빠른 시작

### 1. 기본 실행 (어제 날짜, main 브랜치 기준)

```bash
cd .github/scripts/devlog_klingo
python devlog_generate.py --config config.json
```

### 2. 특정 날짜 DevLog 생성

```bash
python devlog_generate.py --date 2025-11-20 --config config.json
```

### 3. 특정 개발자 브랜치로 생성

```bash
# 특정 개발자의 작업만 보고 싶을 때
python devlog_generate.py --date 2025-11-20 --branch dopple --config config.json
python devlog_generate.py --date 2025-11-20 --branch cubee --config config.json
```

---

## 📁 생성되는 파일

```
Documents/DevLog/Daily/
├── 2025-11-20.md              # DevLog 본문
└── 2025-11-20.metrics.json    # 메트릭 데이터
```

---

## 🎯 DevLog 내용

각 DevLog는 다음 항목을 포함합니다:

1. **오늘의 주요 변경** - 커밋 내용 기반 핵심 변경사항 요약
2. **시스템 영향도** - 변경된 시스템별 영향도 분석
3. **리스크 및 문제** - 잠재적 리스크 식별
4. **검증 필요 항목** - 테스트가 필요한 부분
5. **회의 기반 후속 작업** - 회의록과 연계된 작업
6. **관련 Jira 이슈** - 자동 추론된 관련 이슈

---

## ⚙️ 설정

### 환경 변수 (.github/config.env)

```env
# OpenAI API (필수)
OPENAI_API_KEY=sk-...

# Jira API (선택사항)
JIRA_SITE=your-site-name
JIRA_PROJECT=UN
JIRA_EMAIL=your-email@example.com
JIRA_TOKEN=your-token
```

### 설정 파일 (config.json)

```json
{
  "use_gpt": true,           // GPT 분석 활성화
  "gpt_model": "gpt-4o",     // 사용할 GPT 모델
  "output_dir": "Documents/DevLog/Daily"
}
```

---

## 📊 입력 데이터 소스

DevLog는 다음 6가지 데이터를 통합합니다:

| 소스 | 경로 | 설명 |
|------|------|------|
| Git Diff | - | KST 기준 하루 동안의 커밋 |
| 커밋 메시지 | - | Conventional Commits 형식 |
| 회의록 | `Documents/Meeting/Daily/` | 프론트매터 포함 Markdown |
| 개발자 정보 | `.github/data/developers.csv` | 브랜치-개발자-Jira 매핑 |
| Jira 룰 | `.github/data/jira_map_rules.yaml` | 파일-시스템 매핑 |
| Jira API | - | 작업자별 최근 이슈 |

---

## 🔧 고급 사용법

### 여러 날짜 일괄 생성

**Windows (PowerShell):**
```powershell
cd .github/scripts/devlog_klingo
10..20 | ForEach-Object {
    $date = "2025-11-" + $_.ToString("00")
    python devlog_generate.py --date $date --branch dopple --config config.json
}
```

**Linux/Mac:**
```bash
cd .github/scripts/devlog_klingo
for i in {10..20}; do
    python devlog_generate.py --date 2025-11-$i --branch dopple --config config.json
done
```

### GPT 없이 실행 (Fallback 모드)

config.json에서 `"use_gpt": false`로 설정하면 기본 템플릿으로 생성됩니다.

---

## 🛠️ 개발자 정보 관리

### developers.csv 편집

```csv
branch,developer,jira_account
dopple,배주백,jubaeck Bae
cubee,이은희,이은희
CookieAndJuice,이치훈,이치훈
```

새로운 개발자를 추가하려면:
1. `.github/data/developers.csv` 파일 열기
2. 새 행 추가 (브랜치명, 개발자명, Jira 계정)
3. 저장

---

## 🗺️ Jira 매핑 룰 수정

### jira_map_rules.yaml 편집

```yaml
Network:
  - "Source/Onepiece/Network/"

Voice:
  - "Source/Onepiece/Voice/"

# 새 시스템 추가
MyNewSystem:
  - "Source/Onepiece/MyNewSystem/"
```

파일 경로 패턴을 시스템명으로 매핑합니다.

---

## 📝 자동화

### GitHub Actions 연동 (예정)

`.github/workflows/daily-devlog.yml`:

```yaml
name: Daily DevLog

on:
  schedule:
    - cron: '0 0 * * *'  # 매일 자정 (UTC)

jobs:
  generate:
    runs-on: ubuntu-latest
    steps:
      - uses: actions/checkout@v3
      - name: Generate DevLog
        run: |
          cd .github/scripts/devlog_klingo
          python devlog_generate.py --config config.json
```

---

## 🐛 문제 해결

### "회의록을 찾을 수 없습니다"

- 회의록 파일명 형식 확인: `Meeting_YYMMDD_Daily.md`
- 날짜가 정확한지 확인

### "Jira API 호출 실패"

- `.github/config.env`에서 API 정보 확인
- Jira 토큰이 유효한지 확인

### "커밋이 없습니다"

- 해당 날짜에 실제 커밋이 있는지 확인
- 브랜치명이 정확한지 확인

---

## 📚 참고 문서

- **상세 가이드**: `.github/scripts/devlog_klingo/README.md`
- **원본 요구사항**: `Documents/DevLog/AgentLog/KLingo_DailyDevLog_Guide.md`
- **시스템 구조**: `.github/scripts/devlog_klingo/` 디렉토리 참조

---

## 💡 팁

1. **매일 자동 실행**: 스케줄러나 GitHub Actions로 자동화
2. **팀원과 공유**: DevLog를 Confluence나 Slack으로 전송
3. **메트릭 분석**: `.metrics.json` 파일로 통계 시각화
4. **GPT 모델 선택**: 비용과 품질을 고려하여 `gpt-4o` 또는 `gpt-4o-mini` 선택

---

## 📞 문의

시스템 문제나 개선 제안은 DevOps 팀에 문의하세요.

**생성 일시**: 2025-11-21
**버전**: 1.0.0
