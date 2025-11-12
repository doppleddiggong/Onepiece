# HonKit DevLog Pipeline Stabilization

**작성일**: 2025-11-12  
**작성자**: Claude AI Assistant  
**카테고리**: System / DevOps  
**심각도**: Medium

---

## 📋 목차
1. [문제 개요](#1-문제-개요)
2. [시스템 아키텍처](#2-시스템-아키텍처)
3. [문제 원인 분석](#3-문제-원인-분석)
4. [해결 방안](#4-해결-방안)
5. [코드 수정 내역](#5-코드-수정-내역)
6. [테스트 가이드](#6-테스트-가이드)
7. [학습 포인트](#7-학습-포인트)

---

## 1. 문제 개요

### 1.1 증상
- **현상**: DevLog/Meeting 기준 HonKit 빌드가 `docs` 브랜치를 체크아웃하려다 실패하며 특히 `gh-pages` 푸시 단계에서 리모트 커밋 불일치 에러가 발생하여 최신 페이지가 배포되지 않음.
- **재현**: Daily/Weekly report 자동 생성 → `honkit-build` dispatch → deploy job에서 `peaceiris/actions-gh-pages@v3`가 `cannot lock ref 'refs/heads/gh-pages'` 에러 출력.
- **영향**: Discord 알림과 HonKit 빌드는 돌아가나 실제 gh-pages 사이트는 이전 상태로 남음, 시니어/주니어/PM 모두 오늘의 산출물 확인 불가능.

### 1.2 관련 시스템
- GitHub Actions: `.github/workflows/daily-report.yml`, `.github/workflows/weekly-report.yml`, `.github/workflows/honkit.yml`
- Metrics generation: `.github/scripts/devlog/generate_daily.py`
- HonKit pipeline: `.github/scripts/devlog/update_devlog.py` + `Documents/DevLog`
- Deployment: `peaceiris/actions-gh-pages@v3`

---

## 2. 시스템 아키텍처

### 2.1 Pipeline 구조

```
Metrics JSON → update_devlog.py → Daily/Weekly MD update
      ↓                ↓
 Meeting keywords    GPT prompts
           ↘       HonKit build → gh-pages deploy → Discord notify
```

이 파이프라인은 `main` 브랜치 상태를 기준으로 하며, Daily/Weekly 워크플로가 metrics 기반 update_devlog.py를 먼저 실행하고 그 결과를 HonKit으로 빌드, gh-pages로 배포한 뒤 Discord로 “갱신 섹션” 알림을 보냅니다.

---

## 3. 문제 원인 분석

### 3.1 핵심 문제
`docs` 브랜치 동기화 때문에 deploy 단계가 겹쳐지고 푸시 충돌이 생겨 매번 배포가 실패, 결과적으로 리포트가 실시간으로 확인되지 않음.

### 3.2 상세 분석

#### 3.2.1 브랜치 관리
- 워크플로에서 `.github/workflows/honkit.yml`은 이전에 `git checkout docs`를 수행; capture stage에서 Honors `docs`와 `main` 간 커밋 불일치로 푸시가 거부됨.

#### 3.2.2 deployment conflict
- `peaceiris/actions-gh-pages@v3` 기본은 원격 브랜치의 latest를 기준으로 업데이트: 예상 SHA(7affefc…)와 실제 원격(84fe…)가 달라 `cannot lock ref` 에러 발생.

#### 3.2.3 DevLog 문서 동기화
- Daily/Weekly report scripts는 `docs` 브랜치에 커밋하고 dispatch를 보냈지만 honkit workflow는 다른 브랜치(`main`)을 기준으로 했기 때문에, 실제 Docs 결과는 다시 불일치.

---

## 4. 해결 방안

### 4.1 해결 전략
- `docs` 브랜치 의존 제거 → 모든 자동화는 `main` 경유, `gh-pages`는 HonKit 빌드 결과만을 push.
- `update_devlog.py`로 metrics/meeting 기반 자동 갱신 + GPT 보충.

### 4.2 해결 로직
- Daily & Weekly workflows에서 `git pull origin main` 후 metrics JSON을 업데이트.
- `.github/scripts/devlog/update_devlog.py`는 sections만 교체(`update_in_place`)하고 GPT 프롬프트 템플릿(👍)을 통해 “왜 중요한가” 문장을 채웁니다.
- `meeting-broadcast.yml`은 `latest_file` 포함 dispatch → notify job이 회의용 webhook 사용하여 “회의록 @{file}”을 알림.

### 4.3 해결 후 플로우
- Daily: metrics JSON + meeting note + GPT → `Documents/DevLog/Daily/{{date}}.md`.
- Weekly: 7일간 aggregated metrics → `Documents/DevLog/Weekly/{{range}}.md`.
- HonKit: build → gh-pages main deploy → notify with section names.

---

## 5. 코드 수정 내역

### 5.1 수정 파일
- `.github/scripts/devlog/update_devlog.py`
- `.github/scripts/devlog/prompt_templates/*.txt`
- `.github/workflows/{daily-report,weekly-report,honkit}.yml`
- `Documents/meeting/guide*.md`

### 5.2 수정 위치
- Daily workflow: `Update Daily DevLog...` step
- Weekly workflow: `Update Weekly DevLog...` step
- HonKit workflow: removed docs checkout, added notify job

### 5.3 수정 전 코드
`honkit.yml`은 `git fetch origin docs` → `git checkout docs`, `meeting-broadcast` payload lacked latest_file, daily/weekly workflows built on docs branch.

### 5.4 수정 후 코드
- Workflows target `main`, call `update_devlog.py` before HonKit build.
 - Script implements GPT prompts and meeting linking.

### 5.5 핵심 변경 사항
1. Metrics→GPT integration before deploying.
2. Docs branch removed from pipeline, main-only flow.
3. Meeting note template + prompt bridging with DevLog sections.

---

## 6. 테스트 가이드

### 6.1 테스트 시나리오
1. Local metrics generation + `python update_devlog.py --mode daily --date 2025-11-12` → verify `Documents/DevLog/Daily/2025-11-12.md` sections updated.
2. Run daily workflow via `workflow_dispatch` → check `gh-pages` deploy log for new commit.
3. Modify meeting template, run update script, ensure `## 3. 회의 연계 분석` reflects keywords.

### 6.2 디버깅 로그 체크리스트
- `update_devlog.py` console output: `Updated Documents/DevLog/...`
- Workflow log: HonKit build success, `notify` job mentions updated sections.

### 6.3 Output Log 필터링
- `honkit.yml` notify job prints contained `MSG`.
- `Discord notify` message indicates which section (Mermaid, 회의 연계) changed.

---

## 7. 학습 포인트

### 7.1 자동화와 의미
Metrics 수치만 넣는 자동화는 소음이지만, `GPT 요약 + 기존 마크다운 유지` 방식은 실제 human-readable 회고를 만들 수 있습니다.

### 7.2 회의 연결
회의록 키워드와 metrics의 교집합을 GPT가 설명하면 “회의 → 코드 → 리포트” 라는 팀 피드백 루프가 실현됩니다.

---

## 8. 결론

### 8.1 요약
- **문제**: docs branch 충돌로 gh-pages deploy 실패.
- **원인**: 워크플로 의존 명확하지 않고 Discord/meeting 흐름이 분리됨.
- **해결**: main branch 기반 파이프라인, update_devlog.py + meeting link.
- **효과**: 수치+GPT+회의 로그가 자동 갱신된 DevLog로 팀 리포트 성과 개선.

### 8.2 핵심 교훈
1. 브랜치를 줄이면 파이프라인 안정성은 급격히 올라간다.
2. 자동화는 “의미를 갱신”해야 팀이 읽는다.

---

**문서 버전**: 1.0  
**최종 수정일**: 2025-11-12

**참조 파일**:
- `.github/scripts/devlog/update_devlog.py`
- `.github/workflows/daily-report.yml`
- `Documents/meeting/guide.template.md`
