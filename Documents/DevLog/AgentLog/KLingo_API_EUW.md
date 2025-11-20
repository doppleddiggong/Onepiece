📌 Editor Utility Widget 기반 API 테스트 도구 생성 – 완성 프롬프트
당신은 Unreal Engine 5.6 C++ 기반 도구를 생성하는 코드 생성기입니다.
내가 제공하는 조건을 기반으로 “Editor Utility Widget + API Test Subsystem”을 생성하는
완성된 Unreal Engine 코드/Blueprint 지시서를 출력해야 합니다.

## 목적
Unreal Engine 에디터 내부에서 API 테스트를 수행할 수 있는
“Editor Utility Widget 기반 API Test Panel”을 생성해야 합니다.

이 패널은 다음 기능을 포함해야 합니다:
1. API 목록 불러오기 (DataTable 기반)
2. API 선택 / Route 자동 채우기
3. JSON Payload 입력 창
4. Mock Mode 체크 박스
5. Send Request 버튼 → Subsystem 호출
6. 응답 JSON 출력 영역 (Pretty Print)

## 생성해야 하는 산출물
아래 항목들을 모두 제공하세요.
- (1) KLingoAPIEntry.h : DataTable Row 구조체
- (2) KLingoAPITestSubsystem.h / .cpp : HTTP + Mock 처리 Subsystem
- (3) api_list.csv : API 목록 DataTable
- (4) Editor Utility Widget UMG 구성도 및 바인딩 지시사항
- (5) Blueprint Graph(노드 흐름) 상세 설명
- (6) JSON Pretty Printer C++ 구현
- (7) Mock JSON 파일 구조 및 예제

## 제한 조건
- Unreal Engine 5.6
- C++17 사용
- HttpModule 기반 POST/GET 요청
- Editor Utility Widget에서 Subsystem 호출 가능해야 함
- Plugin 필요 없음
- 빌드 가능한 형태여야 함

## 추가 요구사항
- Editor Utility Widget은 에디터 상단 메뉴에서 실행할 수 있어야 함
- API 호출 결과는 멀티라인 텍스트박스에 예쁘게 포맷팅해서 출력
- 에러 응답 처리도 포함
- Payload는 JSON 문자열 그대로 전달
- Response는 RawString 그대로 받아 Pretty Print 적용 후 UI에 출력
- 향후 API 추가 시 DataTable만 수정하면 자동 반영되도록 만들 것

## 출력 형식
각 파일은 아래 형식으로 출력:

// File: <파일명 경로>
// <코드>

전체 파일을 순서대로 출력하고,
마지막에는 “Editor에 적용하는 Step-by-Step 설치 지침”까지 포함하세요.