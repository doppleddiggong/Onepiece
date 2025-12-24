# KLingo 위젯 애니메이션 시스템 구현 계획

**작성일**: 2025-12-24
**목적**: Unity Coroutine 스타일의 순차 위젯 애니메이션 시스템 구현
**대상 팝업**: Popup_Result 등 동적 위젯 생성 팝업

---

## 📋 요구사항

### 기능 요구사항
1. **순차 애니메이션**: 위젯들이 0.1초 간격으로 순차적으로 등장
2. **Coroutine 시스템**: 함수와 딜레이를 받아 순차 실행
3. **탄성 효과**: UBasePopup과 동일한 EaseOutBack 애니메이션
4. **유연성**: 중첩 실행 지원 (애니메이션 오버랩)
5. **안정성**: 위젯 파괴 시 자동 정리

### 비기능 요구사항
- 기존 UDelayTaskManager 재사용
- FEaseHelper 활용
- Blueprint 지원
- UBasePopup 패턴 일관성 유지

---

## 🏗️ 아키텍처 설계

### 컴포넌트 구조

```
┌─────────────────────────────────────────────┐
│         UCoroutineSystem                    │
│  (CoffeeLibrary/Features)                   │
│  - AddStep(delay, action)                   │
│  - Start() / Stop()                         │
│  - UDelayTaskManager 통합                   │
└─────────────────┬───────────────────────────┘
                  │ 사용
┌─────────────────▼───────────────────────────┐
│      UWidgetAnimationLibrary                │
│  (Onepiece/UI)                              │
│  - SequenceAnimateWidgets()                 │
│  - CreateCoroutineSystem()                  │
└─────────────────┬───────────────────────────┘
                  │ 사용
┌─────────────────▼───────────────────────────┐
│         UAnimUserWidget                     │
│  (Onepiece/UI)                              │
│  - StartAnim() / StopAnim()                 │
│  - NativeTick() 기반 애니메이션             │
│  - FEaseHelper 사용                         │
└─────────────────┬───────────────────────────┘
                  │ 상속
┌─────────────────▼───────────────────────────┐
│            UAnswerItem                      │
│  (Onepiece/UI)                              │
│  - StartAnim() override (선택)              │
└─────────────────────────────────────────────┘
```

---

## 📁 파일 구조

### 생성할 파일

```
CoffeeLibrary/Features/
├── Public/
│   └── UCoroutineSystem.h              ✨ 새로 생성
└── Private/
    └── UCoroutineSystem.cpp            ✨ 새로 생성

Onepiece/UI/
├── Public/
│   ├── UAnimUserWidget.h               ✨ 새로 생성
│   ├── IWidgetAnimatable.h             ✨ 새로 생성 (선택)
│   └── UWidgetAnimationLibrary.h       ✨ 새로 생성
└── Private/
    ├── UAnimUserWidget.cpp             ✨ 새로 생성
    └── UWidgetAnimationLibrary.cpp     ✨ 새로 생성
```

### 수정할 파일

```
Onepiece/UI/
├── Public/
│   └── UAnswerItem.h                   🔧 수정 (UAnimUserWidget 상속)
└── Private/
    └── UAnswerItem.cpp                 🔧 수정 (StartAnim 구현)

Onepiece/MessageBox/Private/
└── Popup_Result.cpp                    🔧 수정 (Coroutine 사용)
```

---

## 🔄 구현 단계

### Phase 1: UCoroutineSystem (CoffeeLibrary)

**목표**: Unity Coroutine 스타일의 순차 실행 시스템

#### 1.1 FCoroutineStep 구조체
```cpp
struct FCoroutineStep
{
    float Delay;                    // 실행 전 대기 시간
    FCoroutineStepDelegate Action;  // 실행할 함수
    bool bAllowNested;              // 중첩 실행 허용
};
```

#### 1.2 UCoroutineSystem 클래스
```cpp
class UCoroutineSystem : public UObject
{
public:
    void AddStep(float Delay, TFunction<void()> Action, bool bAllowNested = false);
    void Start(float InitialDelay = 0.f);
    void Stop();
    bool IsRunning() const;

    FCoroutineStepDelegate OnCompleted;

private:
    void ExecuteNextStep();

    TArray<FCoroutineStep> Steps;
    int32 CurrentIndex;
    bool bIsRunning;
    FDelayHandle CurrentDelayHandle;
};
```

#### 1.3 핵심 구현 로직
```cpp
void UCoroutineSystem::Start(float InitialDelay)
{
    if (Steps.Num() == 0) return;

    CurrentIndex = 0;
    bIsRunning = true;

    if (UDelayTaskManager* DelayMgr = UDelayTaskManager::Get(GetWorld()))
    {
        CurrentDelayHandle = DelayMgr->Delay(this, InitialDelay, [this]()
        {
            ExecuteNextStep();
        });
    }
}

void UCoroutineSystem::ExecuteNextStep()
{
    if (CurrentIndex >= Steps.Num())
    {
        bIsRunning = false;
        OnCompleted.ExecuteIfBound();
        return;
    }

    const FCoroutineStep& Step = Steps[CurrentIndex];
    Step.Action.ExecuteIfBound();
    CurrentIndex++;

    if (CurrentIndex < Steps.Num())
    {
        const float NextDelay = Steps[CurrentIndex].Delay;
        if (UDelayTaskManager* DelayMgr = UDelayTaskManager::Get(GetWorld()))
        {
            CurrentDelayHandle = DelayMgr->Delay(this, NextDelay, [this]()
            {
                ExecuteNextStep();
            });
        }
    }
    else
    {
        bIsRunning = false;
        OnCompleted.ExecuteIfBound();
    }
}
```

---

### Phase 2: UAnimUserWidget (Onepiece/UI)

**목표**: UBasePopup 스타일의 애니메이션 위젯 베이스

#### 2.1 EAnimationType Enum
```cpp
enum class EAnimationType : uint8
{
    None,
    FadeIn,          // 투명도만
    ScaleIn,         // 크기만
    FadeInScale,     // 투명도 + 크기 (기본)
    SlideFromLeft,
    SlideFromTop
};
```

#### 2.2 UAnimUserWidget 클래스
```cpp
class UAnimUserWidget : public UUserWidget
{
public:
    virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

    virtual void StartAnim();
    virtual void StopAnim();

    FOnAnimCompleted OnAnimCompleted;

protected:
    virtual void UpdateAnimation(float InDeltaTime);

    // Blueprint Animation
    TObjectPtr<UWidgetAnimation> BlueprintAnimation;

    // Script Animation Parameters
    bool bIsAnimating;
    float AnimElapsedTime;
    float AnimDuration = 0.3f;
    EAnimationType AnimationType = EAnimationType::FadeInScale;
    float StartScale = 0.8f;
    float TargetScale = 1.0f;
    float StartOpacity = 0.f;
    float TargetOpacity = 1.f;
    FVector2D AnimPivot = FVector2D(0.5f, 0.5f);
    EEaseType EaseType = EEaseType::EaseOutBack;
};
```

#### 2.3 핵심 구현 로직
```cpp
void UAnimUserWidget::StartAnim()
{
    // Blueprint 애니메이션 우선
    if (BlueprintAnimation)
    {
        PlayAnimation(BlueprintAnimation);
        return;
    }

    // Script 애니메이션 초기화
    SetRenderTransformPivot(AnimPivot);

    if (AnimationType == EAnimationType::FadeIn ||
        AnimationType == EAnimationType::FadeInScale)
    {
        SetRenderOpacity(StartOpacity);
    }

    if (AnimationType == EAnimationType::ScaleIn ||
        AnimationType == EAnimationType::FadeInScale)
    {
        SetRenderScale(FVector2D(StartScale, StartScale));
    }

    bIsAnimating = AnimDuration > KINDA_SMALL_NUMBER;
    AnimElapsedTime = 0.f;

    if (!bIsAnimating)
    {
        SetRenderOpacity(TargetOpacity);
        SetRenderScale(FVector2D(TargetScale, TargetScale));
        OnAnimCompleted.Broadcast();
    }
}

void UAnimUserWidget::UpdateAnimation(float InDeltaTime)
{
    if (!bIsAnimating) return;

    AnimElapsedTime += InDeltaTime;
    const float Alpha = FMath::Clamp(AnimElapsedTime / AnimDuration, 0.f, 1.f);
    const float EasedAlpha = FEaseHelper::Ease(Alpha, EaseType);

    switch (AnimationType)
    {
        case EAnimationType::FadeInScale:
        {
            const float NewOpacity = FMath::Lerp(StartOpacity, TargetOpacity, EasedAlpha);
            const float NewScale = FMath::Lerp(StartScale, TargetScale, EasedAlpha);
            SetRenderOpacity(NewOpacity);
            SetRenderScale(FVector2D(NewScale, NewScale));
            break;
        }
        // 다른 타입들...
    }

    if (Alpha >= 1.f)
    {
        bIsAnimating = false;
        OnAnimCompleted.Broadcast();
    }
}
```

---

### Phase 3: IWidgetAnimatable (선택사항)

**목표**: 순차 애니메이션 인터페이스 정의

```cpp
UINTERFACE(BlueprintType)
class UWidgetAnimatable : public UInterface
{
    GENERATED_BODY()
};

class IWidgetAnimatable
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Widget Animation")
    void ActivateAnimation(int32 Index, int32 TotalCount);

    UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Widget Animation")
    void DeactivateAnimation(int32 Index, int32 TotalCount);
};
```

---

### Phase 4: UWidgetAnimationLibrary (헬퍼)

**목표**: 편리한 사용을 위한 헬퍼 함수

```cpp
class UWidgetAnimationLibrary : public UBlueprintFunctionLibrary
{
public:
    static UCoroutineSystem* SequenceAnimateWidgets(
        UObject* WorldContextObject,
        const TArray<UUserWidget*>& Widgets,
        float IntervalDelay = 0.1f,
        float InitialDelay = 0.f
    );

    static UCoroutineSystem* CreateCoroutineSystem(UObject* WorldContextObject);
};
```

#### 구현
```cpp
UCoroutineSystem* UWidgetAnimationLibrary::SequenceAnimateWidgets(
    UObject* WorldContextObject,
    const TArray<UUserWidget*>& Widgets,
    float IntervalDelay,
    float InitialDelay)
{
    if (!WorldContextObject) return nullptr;

    UCoroutineSystem* Coroutine = NewObject<UCoroutineSystem>(WorldContextObject);

    for (int32 i = 0; i < Widgets.Num(); i++)
    {
        UUserWidget* Widget = Widgets[i];
        if (!IsValid(Widget)) continue;

        if (UAnimUserWidget* AnimWidget = Cast<UAnimUserWidget>(Widget))
        {
            Coroutine->AddStep(IntervalDelay, [AnimWidget]()
            {
                AnimWidget->StartAnim();
            });
        }
    }

    Coroutine->Start(InitialDelay);
    return Coroutine;
}
```

---

### Phase 5: 통합 및 적용

#### 5.1 UAnswerItem 수정

**BEFORE**:
```cpp
class UAnswerItem : public UUserWidget
{
    // ...
};
```

**AFTER**:
```cpp
#include "UAnimUserWidget.h"

class UAnswerItem : public UAnimUserWidget
{
    GENERATED_BODY()

public:
    void InitInfo(...);

protected:
    virtual void StartAnim() override;  // 선택
};
```

#### 5.2 Popup_Result 수정

**BEFORE**:
```cpp
void UPopup_Result::InitWrongList()
{
    VerticalBox->ClearChildren();

    for (int32 i = 0; i < WrongList.Num(); i++)
    {
        UAnswerItem* Item = CreateWidget<UAnswerItem>(this, AnswerItemClass);
        Item->InitInfo(QuestType, i + 1, SD, CorrectData);
        VerticalBox->AddChild(Item);
    }
}
```

**AFTER**:
```cpp
#include "UCoroutineSystem.h"

void UPopup_Result::InitWrongList()
{
    VerticalBox->ClearChildren();

    UCoroutineSystem* Coroutine = NewObject<UCoroutineSystem>(this);

    for (int32 i = 0; i < WrongList.Num(); i++)
    {
        UAnswerItem* Item = CreateWidget<UAnswerItem>(this, AnswerItemClass);
        Item->InitInfo(QuestType, i + 1, SD, CorrectData);
        VerticalBox->AddChild(Item);

        // 순차 애니메이션 (Weak Pointer 사용!)
        TWeakObjectPtr<UAnswerItem> WeakItem = Item;
        Coroutine->AddStep(0.1f, [WeakItem]()
        {
            if (WeakItem.IsValid())
            {
                WeakItem->StartAnim();
            }
        });
    }

    Coroutine->Start();
}
```

---

## ⚠️ 주의사항 및 Best Practices

### 1. Lambda Weak Pointer 필수
```cpp
// ❌ 잘못된 예시 (위젯 파괴 시 크래시)
Coroutine->AddStep(0.1f, [Item]() { Item->StartAnim(); });

// ✅ 올바른 예시
TWeakObjectPtr<UAnswerItem> WeakItem = Item;
Coroutine->AddStep(0.1f, [WeakItem]()
{
    if (WeakItem.IsValid())
        WeakItem->StartAnim();
});
```

### 2. GetWorld() 유효성 체크
```cpp
if (UWorld* World = GetWorld())
{
    if (UDelayTaskManager* DelayMgr = UDelayTaskManager::Get(World))
    {
        // 안전하게 사용
    }
}
```

### 3. Include 순서 준수
```cpp
#include "CoreMinimal.h"
#include "UObject/Object.h"
// 기타 include...
#include "ClassName.generated.h"  // 반드시 마지막!
```

### 4. UPROPERTY 메타 데이터
```cpp
// Blueprint에서 보이지 않게 하려면
UPROPERTY()
bool bIsAnimating;

// Blueprint에서 읽기 전용
UPROPERTY(BlueprintReadOnly, Category = "Animation")
float AnimElapsedTime;

// Blueprint에서 수정 가능
UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
float AnimDuration;
```

---

## 🧪 테스트 계획

### 단위 테스트

#### UCoroutineSystem 테스트
- [ ] AddStep() 호출 후 Steps 배열 확인
- [ ] Start() 호출 시 bIsRunning = true
- [ ] Stop() 호출 시 bIsRunning = false, Steps 비움
- [ ] OnCompleted 델리게이트 발생 확인
- [ ] 중첩 실행 테스트

#### UAnimUserWidget 테스트
- [ ] StartAnim() 호출 시 bIsAnimating = true
- [ ] UpdateAnimation()에서 Alpha 계산 정확성
- [ ] OnAnimCompleted 델리게이트 발생 확인
- [ ] Blueprint 애니메이션 우선 동작 확인

### 통합 테스트

#### Popup_Result 테스트
1. **기본 시나리오**:
   - [ ] 팝업 열기
   - [ ] 위젯들이 0.1초 간격으로 등장하는지 확인
   - [ ] 각 위젯의 탄성 애니메이션 확인
   - [ ] 모든 위젯 등장 후 OnCompleted 확인

2. **Edge Cases**:
   - [ ] 위젯 0개 (빈 리스트)
   - [ ] 위젯 1개
   - [ ] 위젯 10개 이상
   - [ ] 팝업 즉시 닫기 (애니메이션 중단)
   - [ ] 빠르게 여러 번 열고 닫기

3. **메모리 누수 체크**:
   - [ ] 팝업 열고 닫기 100회 반복
   - [ ] Task Manager에서 메모리 증가 없음 확인
   - [ ] Visual Studio Memory Profiler 사용

---

## 📊 성능 지표

### 목표
- **애니메이션 부드러움**: 60 FPS 유지
- **메모리**: Coroutine 인스턴스당 ~200 bytes
- **CPU**: NativeTick 오버헤드 최소화

### 최적화
- 애니메이션 완료 시 Tick 비활성화
- Weak Pointer로 불필요한 참조 제거
- UDelayTaskManager로 Timer 관리 위임

---

## 📚 참고 자료

### 기존 코드 참고
- **UBasePopup**: `Onepiece/MessageBox/Public/UBasePopup.h`
  - OpenAnimation 패턴
  - NativeTick 기반 애니메이션
  - FEaseHelper 사용법

- **UDelayTaskManager**: `LatteLibrary/Manager/Public/UDelayTaskManager.h`
  - Delay() API
  - FDelayHandle 사용법
  - Owner 기반 자동 정리

- **FEaseHelper**: `CoffeeLibrary/Shared/Public/FEaseHelper.h`
  - EEaseType enum
  - Ease() 함수 사용법

### 언리얼 공식 문서
- UMG Animation: https://docs.unrealengine.com/5.0/en-US/umg-animations-in-unreal-engine/
- Delegates: https://docs.unrealengine.com/5.0/en-US/delegates-in-unreal-engine/
- Weak Pointers: https://docs.unrealengine.com/5.0/en-US/weak-pointers-in-unreal-engine/

---

## ✅ 완료 체크리스트

### Phase 1: UCoroutineSystem
- [ ] UCoroutineSystem.h 작성
- [ ] UCoroutineSystem.cpp 작성
- [ ] CoffeeLibrary 빌드 성공
- [ ] 간단한 테스트 (3개 스텝 순차 실행)

### Phase 2: UAnimUserWidget
- [ ] EAnimationType enum 정의
- [ ] UAnimUserWidget.h 작성
- [ ] UAnimUserWidget.cpp 작성
- [ ] FadeInScale 애니메이션 동작 확인

### Phase 3: IWidgetAnimatable
- [ ] IWidgetAnimatable.h 작성
- [ ] UAnimUserWidget에 구현 (선택)

### Phase 4: UWidgetAnimationLibrary
- [ ] UWidgetAnimationLibrary.h 작성
- [ ] UWidgetAnimationLibrary.cpp 작성
- [ ] SequenceAnimateWidgets() 테스트

### Phase 5: 통합
- [ ] UAnswerItem 상속 변경
- [ ] Popup_Result InitWrongList() 수정
- [ ] Onepiece 빌드 성공
- [ ] 에디터에서 동작 확인

### 최종 검증
- [ ] 순차 애니메이션 부드러움 확인
- [ ] 0.1초 간격 타이밍 확인
- [ ] 탄성 효과 EaseOutBack 동작 확인
- [ ] 팝업 즉시 닫기 시 크래시 없음
- [ ] 메모리 누수 없음

---

## 🔄 향후 확장 계획

### 1단계 (현재)
- ✅ 기본 Coroutine 시스템
- ✅ FadeInScale 애니메이션
- ✅ Popup_Result 적용

### 2단계 (선택)
- SlideFromLeft, SlideFromTop 애니메이션 추가
- UCurveFloat 지원 (커스텀 커브)
- ReverseAnim() 추가 (Exit 애니메이션)

### 3단계 (미래)
- AddParallelStep() 병렬 실행
- AddStepIf() 조건부 실행
- WaitForDelegate() 이벤트 대기

---

**작성자**: Claude Agent
**승인자**: [작성 필요]
**구현 시작일**: [작성 필요]
**구현 완료 예정일**: [작성 필요]
