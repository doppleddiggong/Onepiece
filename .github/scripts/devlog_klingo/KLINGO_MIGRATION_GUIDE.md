# KLingo DevLog 시스템 마이그레이션 가이드

## 📋 개요

기존 DevLog 시스템을 KLingo DevLog 자동 생성 시스템으로 전환하는 가이드입니다.

**완료 날짜**: 2025-11-23
**담당자**: Claude Code Agent

---

## ✅ 완료된 작업

### 1. 새 Workflow 파일 생성

#### Daily DevLog Workflow
- **파일**: `.github/workflows/daily-devlog-klingo.yml`
- **스케줄**: 매일 KST 09:00 (UTC 00:00)
- **스크립트**: `.github/scripts/devlog_klingo/devlog_generate.py`
- **출력**: `Documents/DevLog/Daily/YYYY-MM-DD.md`

**주요 기능**:
- KLingo DevLog 생성 (6가지 데이터 소스 통합)
- GPT 기반 분석 (활성화 가능)
- Jira 연동 (환경 변수 기반)
- SUMMARY.md 자동 업데이트
- HonKit 빌드 트리거
- Confluence 자동 업로드
- Discord 알림

#### Weekly DevLog Workflow
- **파일**: `.github/workflows/weekly-devlog-klingo.yml`
- **스케줄**: 매주 일요일 KST 23:00 (UTC 14:00)
- **스크립트**: `.github/scripts/devlog_klingo/weekly_devlog_generate.py`
- **출력**: `Documents/DevLog/Weekly/YYYY-WXX-Summary.md`

**주요 기능**:
- 주간 통합 분석
- 시스템별 변경 통계
- 성과 및 달성도 평가
- 다음 주 계획 자동 생성
- 개선 제안 사항 도출

### 2. config.json 업데이트

**위치**: `.github/scripts/devlog_klingo/config.json`

**변경 사항**:
- 환경 변수 지원 주석 추가
- Jira 설정 안내 추가
- OpenAI API 키 환경 변수 우선 사용

### 3. 로컬 테스트 완료

**Daily DevLog 테스트**:
```bash
cd .github/scripts/devlog_klingo
python devlog_generate.py --date 2025-11-21 --branch dopple --config config.json
```

**결과**:
- ✅ 커밋 26개 분석 완료
- ✅ 파일 1992개 변경 사항 추적
- ✅ 회의록 통합
- ✅ Jira 시스템 매핑 (6개 시스템)
- ✅ GPT 기반 DevLog 생성 성공

**Weekly DevLog 테스트**:
```bash
cd .github/scripts/devlog_klingo
python weekly_devlog_generate.py --week 2025-W47 --config config.json
```

**결과**:
- ✅ 3일간 활동 분석
- ✅ 74개 커밋 통합
- ✅ 일별 주요 변경 사항 요약
- ✅ 시스템별 변경 통계 생성
- ✅ 주간 성과 및 계획 자동 생성

### 4. Workflow YAML 검증

```bash
python -c "import yaml; yaml.safe_load(open('.github/workflows/daily-devlog-klingo.yml', encoding='utf-8'))"
python -c "import yaml; yaml.safe_load(open('.github/workflows/weekly-devlog-klingo.yml', encoding='utf-8'))"
```

**결과**: ✅ 모든 YAML 파일 유효성 검증 완료

---

## 🚀 다음 단계

### Phase 1: 병렬 실행 (검증)

기존 workflow를 유지하면서 새 workflow를 테스트합니다.

1. **기존 workflow 이름 변경**:
   ```bash
   # 기존 workflow를 비활성화 (옵션)
   git mv .github/workflows/daily-report.yml .github/workflows/daily-report.yml.backup
   git mv .github/workflows/weekly-report.yml .github/workflows/weekly-report.yml.backup
   ```

2. **새 workflow 수동 실행 테스트**:
   - GitHub Actions 탭에서 "KLingo Daily DevLog" workflow 선택
   - "Run workflow" 클릭
   - 날짜와 브랜치 입력 후 실행
   - 결과 확인

3. **출력 비교**:
   - 기존 DevLog와 KLingo DevLog 비교
   - 내용 품질 검증
   - 메트릭 데이터 확인

### Phase 2: 전환

검증이 완료되면 KLingo 시스템으로 완전 전환합니다.

1. **기존 workflow 아카이브**:
   ```bash
   mkdir -p .github/workflows/archive
   git mv .github/workflows/daily-report.yml.backup .github/workflows/archive/
   git mv .github/workflows/weekly-report.yml.backup .github/workflows/archive/
   ```

2. **스케줄 활성화 확인**:
   - `daily-devlog-klingo.yml`: 매일 UTC 00:00
   - `weekly-devlog-klingo.yml`: 매주 일요일 UTC 14:00

3. **커밋 및 푸시**:
   ```bash
   git add .github/workflows/
   git commit -m "feat(workflow): migrate to KLingo DevLog system"
   git push origin main
   ```

### Phase 3: 모니터링

1. **첫 자동 실행 모니터링**:
   - GitHub Actions 탭에서 실행 로그 확인
   - 에러 여부 확인
   - Discord 알림 확인

2. **생성된 DevLog 검토**:
   - `Documents/DevLog/Daily/` 확인
   - `Documents/DevLog/Weekly/` 확인
   - HonKit 페이지 확인

3. **Confluence 업로드 확인**:
   - Confluence 페이지에 자동 업로드되는지 확인

---

## 🔧 설정

### 필수 GitHub Secrets

다음 Secrets가 설정되어 있는지 확인하세요:

```
OPENAI_API_KEY          # GPT 분석용
JIRA_API_TOKEN          # Jira 연동용
DISCORD_WEBHOOK_DEVLOG  # Discord 알림용
```

### config.yml 설정

`.github/config.yml`에서 다음 설정을 확인하세요:

```yaml
gpt:
  daily_enabled: true   # Daily DevLog GPT 분석
  weekly_enabled: true  # Weekly DevLog GPT 분석

jira:
  enabled: true
  site: "your-site-name"
  email: "your-email@example.com"
  project: "UN"

discord:
  enabled: true

confluence:
  enabled: true
```

---

## 📊 기존 시스템과의 차이점

| 항목 | 기존 시스템 | KLingo 시스템 |
|------|------------|--------------|
| **데이터 소스** | Git diff, 커밋 메시지 | Git diff, 커밋, 회의록, 개발자 정보, Jira 룰, Jira API |
| **분석 깊이** | 기본 커밋 로그 | 시스템 영향도, 리스크 식별, 검증 필요 항목 |
| **회의록 통합** | 별도 | 자동 통합 및 후속 작업 추적 |
| **개발자 매핑** | 없음 | developers.csv 기반 자동 매핑 |
| **시스템 분류** | 수동 | jira_map_rules.yaml 기반 자동 분류 |
| **메트릭** | 기본 | 상세 메트릭 (.metrics.json) |

---

## 🐛 문제 해결

### "Jira API 호출 실패" (410 Error)

**원인**: Jira API 엔드포인트 변경
**해결**:
- Jira API 토큰이 유효한지 확인
- `.github/config.yml`의 Jira 설정 확인
- 비critical 에러이므로 DevLog는 정상 생성됨

### "GPT API 호출 실패"

**원인**: OpenAI API 키 문제
**해결**:
- GitHub Secrets에 `OPENAI_API_KEY` 확인
- API 키 사용량 한도 확인
- Fallback 모드: `config.yml`에서 `gpt.daily_enabled: false` 설정

### "회의록을 찾을 수 없습니다"

**원인**: 회의록 파일명 형식 불일치
**해결**:
- 회의록 파일명이 `Meeting_YYMMDD_Daily.md` 형식인지 확인
- `Documents/Meeting/Daily/` 디렉토리 확인

---

## 📞 지원

문제가 발생하면 다음을 확인하세요:

1. **GitHub Actions 로그**: 상세한 에러 메시지 확인
2. **로컬 테스트**: 위의 테스트 명령어로 로컬에서 재현
3. **설정 파일**: `config.yml`, `config.json`, `config.env` 확인

---

## 📝 변경 이력

- **2025-11-23**: KLingo DevLog 시스템 초기 구축 완료
  - daily-devlog-klingo.yml 작성
  - weekly-devlog-klingo.yml 작성
  - 로컬 테스트 완료
  - YAML 검증 완료

---

**생성**: 2025-11-23
**버전**: 1.0.0
