# 회의 연동 Daily DevLog 템플릿 (Codex/GPT 용)

이 템플릿은 `Documents/meeting/guide.md`를 참고하여 자동화된 DevLog 생성기가 매일 회의 내용을 읽고 `## 3. 회의 연계 분석` 섹션을 만든 뒤에 사용할 수 있도록 구성된 예시입니다.

```
# 회의록 — {{date}}

## 개요
- 프로젝트: {{project}}
- 담당: {{attendees}}
- 참석자: {{attendees_list}}
- 키워드: {{keywords}}  # 헥스, HonKit, Discord 등 주요 키워드를 쉼표 구분

## 논의 Summary
1. {{topic_1}} — 담당 {{owner_1}} / 결정: {{decision_1}}
2. {{topic_2}} — 요약: {{summary_2}}

## Action Items
- [ ] {{action_1}} (담당: {{owner_1}}, 기한: {{due_1}})
- [ ] {{action_2}} (담당: {{owner_2}})

## 질문 / 리스크
- {{risk_1}}
- {{question_1}}

## Reflection
- 잘된 점: {{wins}}
- 미진한 점: {{blockers}}
- 다음 스텝: {{next_steps}}
```

Codex/GPT 파이프라인은 위 파일을 채운 회의록에서 `keywords`, `Action Items`, `Reflection`을 추출하여 Daily DevLog의 `metrics`와 교차시키고, 자동 해석 섹션(`## 3. 회의 연계 분석`)에 반영할 수 있습니다. Daily/Weekly 지표와 함께 이 템플릿을 채워두면 “회의 → 커밋 → 회고” 순환이 완성됩니다.
