# HonKit Deployment/DevLog Pipeline Conflict

**작성일**: 2025-11-12  
**작성자**: Claude AI Assistant  
**카테고리**: DevOps / DevLog  
**심각도**: Medium

---

## 개요
- **현상**: `honkit.yml` deploy 단계가 `peaceiris/actions-gh-pages@v3`에서 `cannot lock ref 'refs/heads/gh-pages'` 오류로 실패해 gh-pages가 최신 DevLog를 반영하지 못함.  
- **원인**: 워크플로 간 브랜치/commit 기준이 달라 `docs` 브랜치에서 honkit을 실행하고, 이후 `main` 기준으로 `_book`을 build하려 하면서 SHA 불일치가 발생함.  
- **영향**: DevLog/GPT pipeline은 동작해도 실제 gh-pages에는 반영되지 않아 PM/팀이 최신 리포트를 확인할 수 없음. Discord 알림은 발송되지만 링크가 오래된 상태임.

---

## 작업 기록
1. `.github/workflows/honkit.yml`에서 `docs` fetch/checkout 제거 → 모든 빌드는 main을 기준으로 실행하게 변경.  
2. `meeting-broadcast.yml`/`daily-report.yml`/`weekly-report.yml`의 repository dispatch 및 deploy 대상 ref를 `main`으로 통일.  
3. `.github/scripts/devlog/update_devlog.py`를 추가해 metrics+meeting 기반 DevLog를 GPT로 증분 업데이트 → honkit build와 연동.  
4. `Documents/meeting/guide*.md`과 README에 DevLog intelligence 파이프라인 설명을 추가하여 정리.

---

## 배포 안정화 방안
- `honkit` deploy job은 gh-pages upstream 상태를 강제로 override하지 않고 원격과 일치시키기 위해 `main` 기준 결과만 push.  
- Deployment 충돌 시 `peaceiris/actions-gh-pages@v3`에 `force_orphan: true` 고려, 필요시 `git fetch origin gh-pages` 후 푸시.
- 로컬에서 `Documents`의 `_book`을 `honkit build`로 재생성하고 `gh-pages` 브랜치를 수동으로 확인하여 연속성을 검증.

---

## 향후 조치
1. `gh-pages` branch protection/자동 정리 정책 검토 (force push 허용 여부).  
2. `meeting-broadcast` → honkit notify path에 “디스코드 메시지가 실제 배포 commit”과 매칭되는지 로그로 남기기.  
3. 자동화 로직과 devlog pipeline을 분리해 두면서도, 배포 실패 감지 시 Alert으로 전파하도록 health check 스크립트를 `Tools/`에 추가.

---

## 결론
- **문제**: DevLog pipeline 빌드 성공에도 `gh-pages` 배포 실패  
- **원인**: docs/main 간 브랜치 불일치  
- **해결**: 모든 워크플로우를 `main` 기준으로 통합하고 update_devlog → honkit → gh-pages 순으로 안정화  
- **효과**: metrics 기반 DevLog가 gh-pages에 연속적으로 반영되어 PM/팀이 매일 확인할 수 있음
