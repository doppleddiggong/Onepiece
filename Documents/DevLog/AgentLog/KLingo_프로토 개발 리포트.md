# 2025-11-10 ~ 2025-12-08 개발 리포트

## 1. 개요
- **기간**: 2025-11-10 ~ 2025-12-08 (KST)
- **데이터 소스**: 일일/주간 DevLog, 데일리 미팅 기록, Git 커밋 로그
- **주요 완료 상태**:  
  - ReadQuest **100%**  
  - ListenQuest **75%**  
  - SpeakQuest **50%**  
  - WriteQuest **50%**  
  (Speak/Write는 플레이 흐름 및 API 연동만 남음)

---

## 2. 기간별 진행 요약

### **W46 (11/10~11/16)**
프로젝트명을 YiSan → Onepiece로 전환, 문서 탐색성 개선, GitHub Actions 안정화,  
AI 보조 도구 도입으로 워크플로 정비 완료.  
【F:Documents/DevLog/Weekly/2025-W46-Summary.md†L10-L39】

### **W47 (11/17~11/23)**
Confluence/Jira 자동화, STT–GPT–TTS 음성 대화 시스템 통합,  
네트워크 API 요청 서브시스템 구축, RadicalMike 등 캐릭터·애니메이션 자산 추가.  
【F:Documents/DevLog/Weekly/2025-W47-Summary.md†L10-L49】

### **W48 (11/24~11/30)**
마스터 데이터 관리, 멀티플레이어 세션/로비 구현, ReadQuest UI 고도화,  
훅/그래플링 시스템 도입, 팝업/메시지 박스 리팩토링, 서버 권한 관리 강화.  
【F:Documents/DevLog/Weekly/2025-W48-Summary.md†L10-L63】

### **W49 (12/01~12/07)**
RichText·TTS·OCR 등 UI/콘텐츠 개선, 결과 팝업/알림 강화,  
SpeakStage 시스템 구축, 사용자 정보 네트워크 동기화 및 캐시 최적화 진행.  
【F:Documents/DevLog/Weekly/2025-W49-Summary.md†L10-L61】

### **12/08 일일 작업**
FoodHolder/Trolley 추가, GameState 멀티캐스트 델리게이트 확장,  
Prop·이벤트 시스템 강화로 상호작용 흐름 보강.  
【F:Documents/DevLog/Daily/2025-12-08.md†L9-L33】

---

## 3. 주요 기능 개발 현황

### **학습 퀘스트**
- ReadQuest **완성**  
- ListenQuest **75%**: 음성 처리·동기화 안정화 진행  
- Speak/WriteQuest **50%**: UI·스테이지 뼈대 구축, API 연동 작업만 남음  
【F:Documents/DevLog/Weekly/2025-W48-Summary.md†L29-L63】  
【F:Documents/DevLog/Weekly/2025-W49-Summary.md†L34-L61】

### **음성/AI 파이프라인**
- STT→LLM→TTS 플로우 통합 및 캐싱  
- Whisper 테스트, TTS 캐시 적용  
- OCR 응답 처리 추가  
【F:Documents/DevLog/Weekly/2025-W47-Summary.md†L12-L32】  
【F:Documents/DevLog/Weekly/2025-W49-Summary.md†L18-L42】

### **네트워크·세션**
- HTTP/소켓 플로우 정리  
- 세션·로비·권한 관리 구축  
- 결과 팝업 동기화 및 사용자 정보 복제 안정화  
【F:Documents/Meeting/Daily/Meeting_251119_Daily.md†L19-L54】  
【F:Documents/DevLog/Weekly/2025-W48-Summary.md†L16-L48】  
【F:Documents/DevLog/Weekly/2025-W49-Summary.md†L44-L58】

### **UI/UX**
- 팝업·메시지 박스 전면 교체  
- RichText/하이퍼링크 적용  
- 결과 팝업 커스텀 위젯화  
- 탭/스크롤 구조 정비, 메인 위젯 탭 연동  
【F:Documents/DevLog/Weekly/2025-W48-Summary.md†L23-L48】  
【F:Documents/Meeting/Daily/Meeting_251204_Daily.md†L15-L41】  
【F:Documents/DevLog/Weekly/2025-W49-Summary.md†L16-L33】

### **콘텐츠/자산**
- RadicalMike 캐릭터  
- 로봇 플레이어  
- 훅 시스템, 플레이어 감지, 텔레포트, 애니메이션  
- 신규 상호작용 오브젝트 추가  
【F:Documents/DevLog/Weekly/2025-W47-Summary.md†L14-L49】  
【F:Documents/DevLog/Weekly/2025-W48-Summary.md†L21-L37】

---

## 4. 이슈 및 대응

### **협동 플레이 설득력 부족 및 데이터 구조 지연**
- LLM 기반 난이도 조정, 힌트 RPC 아이디어 도입  
- 데이터 정의서 기반 단일 구조 확정 추진  
【F:Documents/Meeting/Daily/Meeting_251119_Daily.md†L29-L81】

### **UI/네트워크 동기화 복잡도 증가**
- 팝업 계층 재정리  
- GameMode/GameState 로직 분산  
- Subsystem 도입 검토  
- StepScenarioSystem 구조 확립  
【F:Documents/Meeting/Daily/Meeting_251204_Daily.md†L15-L63】

### **신규 멀티캐스트/Prop 추가 리스크**
- 12/08 이후 시스템 증가로 테스트 범위 확장  
- 이벤트·블루프린트 검증 계획 수립  
- 성능 측정 및 동기화 검증 필요  
【F:Documents/DevLog/Daily/2025-12-08.md†L25-L47】

---

## 5. 브랜치별 커밋 하이라이트 (11/10~12/08)

### **dopple**
- 9620509: 최신 메인 반영  
- f3fe3b9: 메인 변경 재통합  
- 46be53e: 네트워크/자산 업데이트 반영  
- e7e0823: TTS/OCR 개선분 반영  
- 6ffa545: UI/네트워크 리팩토링 포함  
【22a072†L1-L41】

### **CookieAndJuice**
- a961020: 필기 보드 통합  
- 54290ee: 메인 병합 – 최신 기획 반영  
- b4f7276: DrawingBoardWidget 추가  
- 2244647 / a20e997 / 601efd5: 메인 동기화 시리즈  


### **Cubee**
- 8f278d5 / 5c12c21: 메인 병합, 퀘스트/자산 반영  
- 7830146: 결과 팝업/델리게이트 개선  
- 6b389b4: Interactable 13파일 대규모 갱신 (+481/-6)  
- b7903c8~0b1e9f4: 텔레포트/훅/사운드/마스터데이터/오답 UI 추가  
- 91f487f: OrderKiosk 상호작용 및 Food 스폰 연동  
【48a7a4†L10-L13】  
【bb916d†L4-L6】  
【a53bfb†L16-L34】

---

## 6. 다음 단계 및 일정 제안 (12/09 이후)

### **Speak/WriteQuest 완성**
- StepScenarioSystem 흐름에 API 연동 및 동기화 추가  
- 음성·텍스트 검증 루틴 최종 확정

### **성능/안정성 검증**
- 멀티캐스트·세션·팝업 리팩토링 테스트  
- 성능 측정 및 자동화 도입

### **데모 준비**
- 12/09 프로토타입 발표용 데모 시나리오 고정  
- 5분 이내 동영상 캡처 및 발표 스크립트 작성

### **간트차트 반영**
- 12월 2주차: Step1 폴리싱  
- 3주차 이후: Step2/3 확장 및 QA 진행

---
