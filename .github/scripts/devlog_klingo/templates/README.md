# DevLog 템플릿

이 디렉토리에는 DevLog 생성을 위한 템플릿 파일들이 있습니다.

## 파일 설명

### 1. `devlog_template.md`

**용도**: 최종 DevLog 문서의 Markdown 템플릿

**변수**:
- `{{ date }}`: 날짜 (YYYY-MM-DD)
- `{{ developer }}`: 개발자명 또는 "전체 팀"
- `{{ branch }}`: Git 브랜치명
- `{{ commit_count }}`: 커밋 개수
- `{{ files_changed }}`: 변경된 파일 개수
- `{{ generated_time }}`: 생성 시간

**구조**:
```markdown
# Daily DevLog — {{ date }}

**개발자**: {{ developer }}
...

## 1. 오늘의 주요 변경
## 2. 시스템 영향도
## 3. 리스크 및 문제
## 4. 검증 필요 항목
## 5. 회의 기반 후속 작업
## 6. 관련 Jira 이슈
```

### 2. `devlog_prompt.txt`

**용도**: GPT에게 전달하는 프롬프트 템플릿

**변수**:
- `{{ date }}`: 날짜
- `{{ developer }}`: 개발자명
- `{{ meeting_summary }}`: 회의록 요약
- `{{ commits_summary }}`: 커밋 메시지 요약
- `{{ files_count }}`: 변경 파일 개수
- `{{ files_list }}`: 변경 파일 목록
- `{{ systems_summary }}`: 시스템별 변경 요약
- `{{ jira_summary }}`: Jira 이슈 요약

**역할**:
- GPT에게 DevLog 작성 지침 제공
- 6개 섹션 형식 명시
- 출력 품질 가이드라인 제공

## 템플릿 커스터마이징

### DevLog 형식 변경

`devlog_template.md`를 수정하여 출력 형식을 변경할 수 있습니다:

```markdown
# 예시: 섹션 순서 변경
## 1. 시스템 영향도
## 2. 오늘의 주요 변경
...
```

### GPT 지침 변경

`devlog_prompt.txt`를 수정하여 GPT의 작성 스타일을 조정할 수 있습니다:

```
# 예시: 더 간결한 출력 요청
- 각 섹션은 3개 항목 이하로 제한
- 불릿 포인트 형식으로 작성
```

## 주의사항

1. **템플릿 변수 유지**: `{{ variable }}` 형식의 변수는 반드시 유지해야 합니다
2. **인코딩**: UTF-8 인코딩 사용
3. **Markdown 형식**: 유효한 Markdown 문법 준수
4. **백업**: 수정 전 원본 템플릿 백업 권장

## 템플릿 테스트

템플릿 수정 후 테스트:

```bash
cd .github/scripts/devlog_klingo
python devlog_generate.py --date 2025-11-20 --config config.json
```

생성된 DevLog를 확인하여 형식이 올바른지 검증하세요.
