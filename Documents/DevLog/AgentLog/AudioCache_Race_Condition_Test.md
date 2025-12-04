# Audio Cache Race Condition Test Scenarios

**날짜**: 2025-12-02
**목적**: OnClickRepeat 연속 클릭 시 캐시 및 재생 문제 재현

---

## 테스트 시나리오

### 시나리오 1: 같은 단어 연속 클릭 (중복 요청)
```
1. "Hello" 팝업 열림 → 자동 재생 시작
2. 사용자가 Repeat 버튼 빠르게 3번 클릭
3. RequestListenAudio("Hello") 4번 호출 (초기 1회 + 클릭 3회)
```

**예상 문제**:
- ❌ 캐시 MISS → 4개의 네트워크 요청 발생
- ❌ 4개의 응답이 모두 도착 → 오디오 4번 재생 (덮어쓰기)

**기대 동작**:
- ✅ 첫 요청만 네트워크 전송
- ✅ 나머지 3개는 요청 중 플래그로 차단 OR 대기열 관리
- ✅ 캐시 저장 후 한 번만 재생

---

### 시나리오 2: 다른 단어 빠르게 전환
```
1. "Hello" 팝업 열림 → RequestListenAudio("Hello") 호출 → 네트워크 대기
2. 빠르게 팝업 닫고 "World" 클릭
3. "World" 팝업 열림 → RequestListenAudio("World") 호출 → 네트워크 대기
4. "World" 응답 먼저 도착 → "World" 재생 시작
5. "Hello" 응답 늦게 도착 → "World" 재생 중인데 "Hello" 재생 (덮어쓰기)
```

**예상 문제**:
- ❌ "World" 재생 중 "Hello"가 끼어들어 재생
- ❌ 사용자 혼란: "World"를 보고 있는데 "Hello" 소리

**기대 동작**:
- ✅ 새 요청이 오면 이전 재생 중단
- ✅ 또는 팝업이 닫히면 pending 요청 취소

---

### 시나리오 3: 네트워크 응답 순서 뒤바뀜
```
1. "Apple" 요청 → 네트워크 A (느림)
2. "Banana" 요청 → 네트워크 B (빠름)
3. 응답 B 먼저 도착 → "Banana" 재생
4. 응답 A 나중에 도착 → "Apple" 재생 (팝업은 "Banana" 표시 중)
```

**예상 문제**:
- ❌ 팝업과 오디오 불일치

**기대 동작**:
- ✅ 현재 팝업의 단어와 일치하는 응답만 재생
- ✅ 또는 요청 ID 추적하여 최신 요청만 처리

---

### 시나리오 4: 캐시 HIT 시 즉시 재생
```
1. "Hello" 첫 클릭 → 캐시 MISS → 네트워크 요청 → 저장
2. "Hello" 두 번째 클릭 → 캐시 HIT → 즉시 재생
```

**예상 동작**:
- ✅ 캐시 HIT 시 즉시 재생 (1ms 이내)
- ✅ 네트워크 요청 없음

**확인 사항**:
- 로그에 `[AudioCache HIT] Hello` 출력되는지
- NetworkWaitCount 증가하지 않는지

---

## 현재 코드 분석

### RequestListenAudio Lambda 캡처
```cpp
Request->OnProcessRequestComplete().BindLambda(
    [WeakThis, InDelegate, AudioText](...)  // ✅ AudioText 값 캡처 (안전)
    {
        // ...
        CacheManager->SaveToCache(AudioText, ResponseData.audio_base64);  // ✅ 올바른 키 사용
    });
```

**결론**: Lambda는 안전하게 구현되어 있음

### UPopup_Word::OnClickRepeat
```cpp
void UPopup_Word::OnClickRepeat()
{
    if (auto KLingoNetwork = UKLingoNetworkSystem::Get(GetWorld()))
    {
        // ❌ 중복 요청 방지 없음!
        KLingoNetwork->RequestListenAudio(
            Data.Kor,  // ⚠️ 멤버 변수 사용
            FResponseListenAudioDelegate::CreateUObject(this, &UPopup_Word::OnResponseListenAudio)
        );
    }
}
```

**문제점**:
1. 중복 클릭 방지 없음
2. 이전 요청 취소 로직 없음
3. `Data.Kor`가 변경될 수 있음 (하지만 RequestListenAudio에서는 값 복사하므로 안전)

---

## 방어 코드 필요 사항

### 1. OnClickRepeat 중복 요청 방지 (우선순위: P0)

**방법 A: 플래그 사용**
```cpp
class UPopup_Word
{
private:
    bool bIsRequestingAudio = false;

void OnClickRepeat()
{
    if (bIsRequestingAudio)
    {
        PRINTLOG(TEXT("[Popup_Word] Already requesting audio, ignoring click"));
        return;
    }

    bIsRequestingAudio = true;
    RequestListenAudio(...);
}

void OnResponseListenAudio(...)
{
    bIsRequestingAudio = false;  // 요청 완료
    // ...
}
```

**방법 B: 요청 ID 추적**
```cpp
class UPopup_Word
{
private:
    FString CurrentRequestText;

void OnClickRepeat()
{
    CurrentRequestText = Data.Kor;
    RequestListenAudio(CurrentRequestText, ...);
}

void OnResponseListenAudio(FResponseListenAudio& ResponseData, ...)
{
    // 현재 팝업의 단어와 일치하는지 확인
    if (ResponseData.audio_text != CurrentRequestText)
    {
        PRINTLOG(TEXT("[Popup_Word] Ignoring outdated response: %s (current: %s)"),
                 *ResponseData.audio_text, *CurrentRequestText);
        return;
    }
    // ...
}
```

### 2. 서버 응답 검증 (우선순위: P1)

**RequestListenAudio Lambda에서**:
```cpp
if (ResponseData.audio_text != AudioText)
{
    NETWORK_LOG(TEXT("[WARNING] Server returned different audio_text! Requested: %s, Got: %s"),
                *AudioText, *ResponseData.audio_text);
    // 요청한 키로 저장 (Lambda 캡처값 사용)
}
CacheManager->SaveToCache(AudioText, ResponseData.audio_base64);
```

### 3. 오디오 재생 중단 처리 (우선순위: P2)

**UPopup_Word에서**:
```cpp
void OnClickRepeat()
{
    // 이전 재생 중단
    if (auto PlayerActor = ULingoGameHelper::GetPlayerActor(this))
    {
        PlayerActor->StopTTSAudio();
    }

    // 새 요청
    RequestListenAudio(...);
}
```

---

## 테스트 체크리스트

- [ ] 시나리오 1: 같은 단어 연속 클릭 테스트
- [ ] 시나리오 2: 다른 단어 빠르게 전환 테스트
- [ ] 시나리오 3: 네트워크 응답 순서 확인
- [ ] 시나리오 4: 캐시 HIT 동작 확인
- [ ] 로그 확인: `[AudioCache HIT]`, `[AudioCache MISS]`
- [ ] NetworkWaitCount 확인
- [ ] 중복 재생 발생 여부 확인

---

## 다음 단계

1. **문제 재현**: 실제 게임에서 연속 클릭 테스트
2. **로그 수집**: NETWORK_LOG 출력 확인
3. **방어 코드 구현**: OnClickRepeat에 플래그 추가
4. **재테스트**: 방어 코드 적용 후 확인
