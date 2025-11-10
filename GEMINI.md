# Gemini 에이전트 가이드

**[Base Persona]** `AgentRule/PERSONA.md`

*이 파일은 `AgentRule/PERSONA.md`에 정의된 기본 페르소나를 상속받습니다. 기본 파일의 모든 규칙이 적용되며, 아래에는 Gemini 에이전트에만 해당하는 특화 지침이 기술되어 있습니다.*

---

## Gemini 특화 지침 (Gemini-Specific Instructions)

*이 섹션에는 Gemini 에이전트 모델에만 해당하는 고유한 지침, 기능 또는 제약 사항을 기술합니다.*


이 저장소는 **Onepiece** 프로젝트입니다. Gemini 에이전트는 다음 문서를 반드시 확인하세요:

- **Onepiece Agent 공통 가이드**: `AgentRule/Project/Onepiece/AGENT_GUIDE.md`
  - 프로젝트 개요, 코딩 컨벤션, Doxygen 정책 등 모든 에이전트가 공유하는 정보를 제공합니다.
- **UE 코딩 컨벤션**: `AgentRule/Project/ue_coding_conventions.md`
  - 언리얼 엔진 전반에 적용되는 규칙이 정리되어 있습니다.

### 작업 시 주의사항

1. **공통 지침 동기화**: 작업 전 `AgentRule/Project/Onepiece/AGENT_GUIDE.md`와 연동 문서를 통해 최신 프로젝트 정책을 확인합니다.
2. **모듈 접근**: `ONEPIECE_API` 매크로와 `LogOnepiece` 카테고리를 사용하며, CoffeeLibrary와 LatteLibrary의 역할을 구분합니다.
3. **문서화**: `AgentRule/Project/Onepiece/DOXYGEN_SETUP.md`를 참고해 Doxygen 공개 문서에서 에이전트 자료가 노출되지 않도록 합니다.
