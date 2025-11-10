# 범용 에이전트 가이드

**[Base Persona]** `AgentRule/PERSONA.md`

*이 파일은 `AgentRule/PERSONA.md`에 정의된 기본 페르소나를 상속받습니다. 기본 파일의 모든 규칙이 적용되며, 아래에는 범용 에이전트에만 해당하는 특화 지침이 기술되어 있습니다.*

---

## 범용 에이전트 특화 지침 (General Agent-Specific Instructions)

*이 섹션에는 특정 모델에 국한되지 않는, 범용 에이전트에게 적용되는 추가 지침을 기술합니다.*


이 저장소는 **Onepiece** 프로젝트입니다. 에이전트는 공통 규칙을 다음 문서에서 확인하세요:

- **Onepiece Agent 공통 가이드**: `AgentRule/Project/Onepiece/AGENT_GUIDE.md`
  - 프로젝트 개요, 코딩 컨벤션, Doxygen 설정에 대한 중앙 허브 역할을 합니다.
- **UE 코딩 컨벤션**: `AgentRule/Project/ue_coding_conventions.md`
  - 모든 언리얼 엔진 프로젝트에 공통으로 적용되는 규칙입니다.

### 범용 Unreal Engine 규칙

모든 Unreal Engine 프로젝트에 공통적으로 적용되는 규칙은 `AgentRule/Project/ue_coding_conventions.md` 문서를 기준으로 합니다.

### 작업 시 주의사항

1. **공통 지침 우선 확인**: 작업 시작 전 `AgentRule/Project/Onepiece/AGENT_GUIDE.md`를 통해 최신 프로젝트 정책을 파악합니다.
2. **모듈명 사용**: `ONEPIECE_API` 매크로와 `LogOnepiece` 로그 카테고리를 유지합니다.
3. **라이브러리 활용**: `CoffeeLibrary`(공용 유틸리티)와 `LatteLibrary`(게임플레이 로직)을 적절히 구분하여 사용합니다.
4. **문서화**: `AgentRule/Project/Onepiece/DOXYGEN_SETUP.md`를 참고해 Doxygen 공개 문서에서 에이전트 자료가 노출되지 않도록 합니다.
