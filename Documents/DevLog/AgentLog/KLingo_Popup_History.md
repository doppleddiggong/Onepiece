# 🚀 Chat History System 구현 명세서

## 📋 프로젝트 정보
- **프로젝트명**: Onepiece (KLingo)
- **엔진 버전**: Unreal Engine 5.4
- **모듈명**: ONEPIECE_API
- **로그 카테고리**: LogOnepiece
- **구현 목표**: AI Chat 대화 기록을 로컬에 저장하고, 히스토리를 조회할 수 있는 시스템 구축

## 🎯 구현 목표
AI Chat 대화 내용(FResponseChatAnswers)을 GConfig를 이용하여 로컬에 저장하고, 팝업을 통해 히스토리를 조회할 수 있는 시스템을 구현합니다.

### 핵심 기능
1. **Chat History 저장**: AI Chat 응답을 받을 때마다 로컬에 자동 저장
2. **유저별 관리**: UserId 기반으로 각 유저의 대화 히스토리를 독립적으로 관리
3. **순서 보장**: 대화 순서(Index)를 부여하여 시간순으로 정렬
4. **히스토리 조회**: 팝업을 통해 저장된 대화 기록을 확인
5. **입력 단축키**: IA_HISTORY 입력으로 히스토리 팝업 열기

---

## 📌 1. 시스템 아키텍처

### 1.1 구성 요소

```
┌─────────────────────────────────────────────────────────────┐
│                      APlayerControl                          │
│  ┌────────────────────────────────────────────────────────┐ │
│  │           UChatHistorySystem (Component)                │ │
│  │  - SaveChatHistory(question, answer)                   │ │
│  │  - LoadAllChatHistory() → TArray<FChatHistoryItem>    │ │
│  │  - ClearChatHistory()                                  │ │
│  └────────────────────────────────────────────────────────┘ │
│                                                               │
│  ┌────────────────────────────────────────────────────────┐ │
│  │  Input: IA_HISTORY                                      │ │
│  │  → OnHistory() → ShowPopup_History()                   │ │
│  └────────────────────────────────────────────────────────┘ │
└─────────────────────────────────────────────────────────────┘
                            │
                            ▼
┌─────────────────────────────────────────────────────────────┐
│                    UPopup_History                            │
│  ┌────────────────────────────────────────────────────────┐ │
│  │  ScrollBox                                              │ │
│  │    └─ VerticalBox                                      │ │
│  │         ├─ HistoryItem 1 (구 데이터)                   │ │
│  │         ├─ HistoryItem 2                               │ │
│  │         └─ HistoryItem N (신 데이터)                   │ │
│  └────────────────────────────────────────────────────────┘ │
│  ┌────────────────────────────────────────────────────────┐ │
│  │  Txt_NoData (데이터 없을 때 표시)                       │ │
│  └────────────────────────────────────────────────────────┘ │
└─────────────────────────────────────────────────────────────┘
                            │
                            ▼
┌─────────────────────────────────────────────────────────────┐
│                  UHistoryItem (Widget)                       │
│  - Txt_Question: "What is your name?"                       │
│  - Txt_Answer: "My name is Alice."                          │
│  - Txt_Timestamp: "2025-12-23 14:30:25"                     │
└─────────────────────────────────────────────────────────────┘
```

---

## 📌 2. 데이터 구조

### 2.1 FResponseChatAnswers (기존 구조체)
`NetworkData.h`에 이미 정의되어 있습니다.

```cpp
USTRUCT(BlueprintType)
struct FResponseChatAnswers
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite, Category = "Chat")
    FString question;

    UPROPERTY(BlueprintReadWrite, Category = "Chat")
    FString answer;
};
```

### 2.2 FChatHistoryItem (신규 구조체)
저장 및 표시를 위한 확장 구조체입니다.

```cpp
/// @brief Chat 히스토리 아이템 구조체입니다.
/// @note GConfig에 저장되며, 순서와 타임스탬프를 포함합니다.
USTRUCT(BlueprintType)
struct FChatHistoryItem
{
    GENERATED_BODY()

    /** 히스토리 인덱스 (0부터 시작, 작을수록 오래된 대화) */
    UPROPERTY(BlueprintReadWrite, Category = "ChatHistory")
    int32 Index = 0;

    /** 질문 내용 */
    UPROPERTY(BlueprintReadWrite, Category = "ChatHistory")
    FString Question;

    /** 답변 내용 */
    UPROPERTY(BlueprintReadWrite, Category = "ChatHistory")
    FString Answer;

    /** 저장 시간 (YYYY-MM-DD HH:MM:SS 형식) */
    UPROPERTY(BlueprintReadWrite, Category = "ChatHistory")
    FString Timestamp;
};
```

**위치**: `Source/Onepiece/Network/Public/NetworkData.h`에 추가

---

## 📌 3. UChatHistorySystem (Component)

### 3.1 클래스 개요

```cpp
/// @file UChatHistorySystem.h
/// @brief Chat 대화 기록을 GConfig를 이용하여 관리하는 컴포넌트입니다.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "NetworkData.h"
#include "UChatHistorySystem.generated.h"

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class ONEPIECE_API UChatHistorySystem : public UActorComponent
{
    GENERATED_BODY()

public:
    UChatHistorySystem();

    /// @brief Chat 대화를 히스토리에 저장합니다.
    /// @param Question [in] 질문 내용
    /// @param Answer [in] 답변 내용
    UFUNCTION(BlueprintCallable, Category = "ChatHistory")
    void SaveChatHistory(const FString& Question, const FString& Answer);

    /// @brief 저장된 모든 Chat 히스토리를 불러옵니다.
    /// @param OutHistoryList [out] 히스토리 리스트 (Index 순으로 정렬됨)
    /// @return 히스토리 개수
    UFUNCTION(BlueprintCallable, Category = "ChatHistory")
    int32 LoadAllChatHistory(TArray<FChatHistoryItem>& OutHistoryList);

    /// @brief 저장된 모든 Chat 히스토리를 삭제합니다.
    UFUNCTION(BlueprintCallable, Category = "ChatHistory")
    void ClearChatHistory();

    /// @brief 현재 유저의 히스토리 개수를 반환합니다.
    UFUNCTION(BlueprintCallable, Category = "ChatHistory")
    int32 GetHistoryCount() const;

private:
    /// @brief GConfig에서 다음 인덱스를 가져오거나 생성합니다.
    int32 GetNextIndex();

    /// @brief 현재 시간을 "YYYY-MM-DD HH:MM:SS" 형식으로 반환합니다.
    FString GetCurrentTimestamp() const;

    /// @brief Config Section 이름을 반환합니다.
    /// @return "/Script/Onepiece.ChatHistory"
    FString GetConfigSection() const;

    /// @brief Config Key를 생성합니다.
    /// @param UserId [in] 유저 ID
    /// @param KeyType [in] "Count", "Question", "Answer", "Timestamp"
    /// @param Index [in] 히스토리 인덱스 (Count일 때는 -1)
    /// @return "ChatHistory_<UserId>_<KeyType>_<Index>"
    FString GetConfigKey(int32 UserId, const FString& KeyType, int32 Index = -1) const;
};
```

**파일 위치**:
- **Header**: `Source/Onepiece/Chat/Public/UChatHistorySystem.h`
- **Source**: `Source/Onepiece/Chat/Private/UChatHistorySystem.cpp`

### 3.2 GConfig 저장 구조

#### Config 파일
- **파일 경로**: `Saved/Config/Windows/GameUserSettings.ini`
- **Section**: `[/Script/Onepiece.ChatHistory]`

#### Key 명명 규칙
```
ChatHistory_<UserId>_Count=<총 저장된 대화 개수>
ChatHistory_<UserId>_Question_<Index>=<질문 내용>
ChatHistory_<UserId>_Answer_<Index>=<답변 내용>
ChatHistory_<UserId>_Timestamp_<Index>=<저장 시간>
```

#### 저장 예시
```ini
[/Script/Onepiece.ChatHistory]
ChatHistory_1_Count=3
ChatHistory_1_Question_0=What is your name?
ChatHistory_1_Answer_0=My name is Alice.
ChatHistory_1_Timestamp_0=2025-12-23 14:25:10
ChatHistory_1_Question_1=How old are you?
ChatHistory_1_Answer_1=I am 25 years old.
ChatHistory_1_Timestamp_1=2025-12-23 14:26:35
ChatHistory_1_Question_2=What is your hobby?
ChatHistory_1_Answer_2=I like reading books.
ChatHistory_1_Timestamp_2=2025-12-23 14:28:42
```

### 3.3 핵심 메서드 구현

#### 3.3.1 SaveChatHistory
```cpp
void UChatHistorySystem::SaveChatHistory(const FString& Question, const FString& Answer)
{
    const int32 UserId = ULingoGameHelper::GetUserId(GetWorld());
    if (UserId <= 0)
    {
        PRINTLOG(TEXT("[ChatHistory] Invalid UserId: %d"), UserId);
        return;
    }

    const int32 NextIndex = GetNextIndex();
    const FString ConfigSection = GetConfigSection();
    const FString Timestamp = GetCurrentTimestamp();

    // 질문 저장
    GConfig->SetString(
        *ConfigSection,
        *GetConfigKey(UserId, TEXT("Question"), NextIndex),
        *Question,
        GGameUserSettingsIni
    );

    // 답변 저장
    GConfig->SetString(
        *ConfigSection,
        *GetConfigKey(UserId, TEXT("Answer"), NextIndex),
        *Answer,
        GGameUserSettingsIni
    );

    // 타임스탬프 저장
    GConfig->SetString(
        *ConfigSection,
        *GetConfigKey(UserId, TEXT("Timestamp"), NextIndex),
        *Timestamp,
        GGameUserSettingsIni
    );

    // Count 업데이트
    GConfig->SetInt(
        *ConfigSection,
        *GetConfigKey(UserId, TEXT("Count")),
        NextIndex + 1,
        GGameUserSettingsIni
    );

    GConfig->Flush(false, GGameUserSettingsIni);

    PRINTLOG(TEXT("[ChatHistory] Saved Index=%d, Q=%s, A=%s"), NextIndex, *Question, *Answer);
}
```

#### 3.3.2 LoadAllChatHistory
```cpp
int32 UChatHistorySystem::LoadAllChatHistory(TArray<FChatHistoryItem>& OutHistoryList)
{
    OutHistoryList.Empty();

    const int32 UserId = ULingoGameHelper::GetUserId(GetWorld());
    if (UserId <= 0)
    {
        return 0;
    }

    const FString ConfigSection = GetConfigSection();
    int32 Count = 0;

    // Count 읽기
    GConfig->GetInt(
        *ConfigSection,
        *GetConfigKey(UserId, TEXT("Count")),
        Count,
        GGameUserSettingsIni
    );

    if (Count <= 0)
    {
        return 0;
    }

    // 모든 히스토리 읽기
    for (int32 i = 0; i < Count; ++i)
    {
        FChatHistoryItem Item;
        Item.Index = i;

        GConfig->GetString(
            *ConfigSection,
            *GetConfigKey(UserId, TEXT("Question"), i),
            Item.Question,
            GGameUserSettingsIni
        );

        GConfig->GetString(
            *ConfigSection,
            *GetConfigKey(UserId, TEXT("Answer"), i),
            Item.Answer,
            GGameUserSettingsIni
        );

        GConfig->GetString(
            *ConfigSection,
            *GetConfigKey(UserId, TEXT("Timestamp"), i),
            Item.Timestamp,
            GGameUserSettingsIni
        );

        OutHistoryList.Add(Item);
    }

    PRINTLOG(TEXT("[ChatHistory] Loaded %d items for User %d"), Count, UserId);
    return Count;
}
```

#### 3.3.3 GetNextIndex
```cpp
int32 UChatHistorySystem::GetNextIndex()
{
    const int32 UserId = ULingoGameHelper::GetUserId(GetWorld());
    const FString ConfigSection = GetConfigSection();
    int32 Count = 0;

    GConfig->GetInt(
        *ConfigSection,
        *GetConfigKey(UserId, TEXT("Count")),
        Count,
        GGameUserSettingsIni
    );

    return Count;
}
```

#### 3.3.4 GetCurrentTimestamp
```cpp
FString UChatHistorySystem::GetCurrentTimestamp() const
{
    const FDateTime Now = FDateTime::Now();
    return FString::Printf(TEXT("%04d-%02d-%02d %02d:%02d:%02d"),
        Now.GetYear(), Now.GetMonth(), Now.GetDay(),
        Now.GetHour(), Now.GetMinute(), Now.GetSecond());
}
```

---

## 📌 4. APlayerControl 통합

### 4.1 헤더 파일 수정

```cpp
// APlayerControl.h

public:
    /// @brief Chat History System에 접근합니다.
    UFUNCTION(BlueprintCallable, Category = "Chat")
    UChatHistorySystem* GetChatHistorySystem() const { return ChatHistorySystem; }

protected:
    /// @brief History 팝업을 표시합니다.
    void OnHistory(const FInputActionValue& Value);

protected:
    // Input Assets
    UPROPERTY(EditDefaultsOnly, Category="Input")
    TObjectPtr<class UInputAction> IA_HISTORY;

    /// @brief Chat History 관리 컴포넌트
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Chat")
    TObjectPtr<UChatHistorySystem> ChatHistorySystem;
```

### 4.2 생성자에서 컴포넌트 생성

```cpp
// APlayerControl.cpp

APlayerControl::APlayerControl()
{
    // ... 기존 코드 ...

    // ChatHistorySystem 컴포넌트 생성
    ChatHistorySystem = CreateDefaultSubobject<UChatHistorySystem>(TEXT("ChatHistorySystem"));
}
```

### 4.3 Input Binding

```cpp
void APlayerControl::SetupInputComponent()
{
    Super::SetupInputComponent();

    // ... 기존 Input 바인딩 ...

    // History 입력 바인딩
    if (IA_HISTORY)
    {
        EnhancedInputComponent->BindAction(IA_HISTORY, ETriggerEvent::Started, this, &APlayerControl::OnHistory);
    }
}
```

### 4.4 OnHistory 구현

```cpp
void APlayerControl::OnHistory(const FInputActionValue& Value)
{
    // ShowPopupAs 템플릿 함수를 사용하여 History 팝업 표시
    if (auto Popup = UPopupManager::ShowPopupAs<UPopup_History>(GetWorld(), EPopupType::History))
    {
        Popup->InitPopup();
    }
}
```

**참고**: `ShowPopupAs<T>()` 템플릿 함수는 팝업을 생성하고 화면에 표시한 뒤, 캐스팅된 포인터를 반환합니다.

### 4.5 Chat 응답 저장

기존 `OnChatAnswerReceived` 함수를 수정하여 히스토리를 저장합니다.

```cpp
void APlayerControl::OnChatAnswerReceived(FResponseChatAnswers& ResponseData, bool bWasSuccessful)
{
    if (bWasSuccessful)
    {
        // ... 기존 Chat 표시 로직 ...

        // Chat History 저장
        if (ChatHistorySystem)
        {
            ChatHistorySystem->SaveChatHistory(ResponseData.question, ResponseData.answer);
        }
    }
}
```

---

## 📌 5. UPopup_History (팝업 위젯)

### 5.1 클래스 선언

```cpp
/// @file UPopup_History.h
/// @brief Chat History를 표시하는 팝업 위젯입니다.

#pragma once

#include "CoreMinimal.h"
#include "UBasePopup.h"
#include "NetworkData.h"
#include "UPopup_History.generated.h"

UCLASS()
class ONEPIECE_API UPopup_History : public UBasePopup
{
    GENERATED_BODY()

public:
    /// @brief 팝업 초기화
    UFUNCTION(BlueprintCallable)
    void InitPopup();

protected:
    virtual void NativeConstruct() override;

private:
    /// @brief 히스토리 아이템을 생성하여 VerticalBox에 추가합니다.
    void RefreshHistoryList();

    UFUNCTION()
    void OnClickClose();

    UFUNCTION()
    void OnClickClear();

protected:
    /* ------------------- Layout ------------------- */

    /** 팝업 타이틀 */
    UPROPERTY(meta = (BindWidget))
    TObjectPtr<class UTextBlock> Txt_Title;

    /** 닫기 버튼 */
    UPROPERTY(meta = (BindWidget))
    TObjectPtr<class UTextureButton> Btn_Close;

    /** 전체 삭제 버튼 */
    UPROPERTY(meta = (BindWidget))
    TObjectPtr<class UImageButton> Btn_Clear;

    /** 스크롤 가능한 히스토리 패널 */
    UPROPERTY(meta = (BindWidget))
    TObjectPtr<class UScrollBox> ScrollBox;

    /** 히스토리 항목들이 추가될 VerticalBox */
    UPROPERTY(meta = (BindWidget))
    TObjectPtr<class UVerticalBox> VerticalBox;

    /** 데이터가 없을 때 표시할 텍스트 */
    UPROPERTY(meta = (BindWidget))
    TObjectPtr<class UTextBlock> Txt_NoData;

    /* ------------------- Classes ------------------- */

    /** 히스토리 항목 위젯 클래스 */
    UPROPERTY(EditDefaultsOnly, Category="Popup")
    TSubclassOf<class UHistoryItem> HistoryItemClass;

    /* ------------------- Settings ------------------- */

    /** 히스토리 항목 간 간격 (Spacer Height) */
    UPROPERTY(EditDefaultsOnly, Category="Popup", meta=(ClampMin="0.0", ClampMax="200.0"))
    float ItemSpacing = 15.0f;
};
```

**파일 위치**:
- **Header**: `Source/Onepiece/MessageBox/Public/UPopup_History.h`
- **Source**: `Source/Onepiece/MessageBox/Private/UPopup_History.cpp`

### 5.2 InitPopup 구현

```cpp
void UPopup_History::InitPopup()
{
    RefreshHistoryList();
}
```

### 5.3 RefreshHistoryList 구현

```cpp
void UPopup_History::RefreshHistoryList()
{
    if (!VerticalBox || !Txt_NoData)
    {
        return;
    }

    // 기존 항목 제거
    VerticalBox->ClearChildren();

    // ChatHistorySystem에서 히스토리 로드
    APlayerControl* PC = ULingoGameHelper::GetPlayerControl(GetWorld());
    if (!PC || !PC->GetChatHistorySystem())
    {
        Txt_NoData->SetVisibility(ESlateVisibility::Visible);
        ScrollBox->SetVisibility(ESlateVisibility::Hidden);
        return;
    }

    TArray<FChatHistoryItem> HistoryList;
    const int32 Count = PC->GetChatHistorySystem()->LoadAllChatHistory(HistoryList);

    if (Count == 0)
    {
        Txt_NoData->SetVisibility(ESlateVisibility::Visible);
        ScrollBox->SetVisibility(ESlateVisibility::Hidden);
        return;
    }

    // 히스토리 아이템 생성 (Index 순서대로 = 상단이 구 데이터, 하단이 신 데이터)
    Txt_NoData->SetVisibility(ESlateVisibility::Hidden);
    ScrollBox->SetVisibility(ESlateVisibility::Visible);

    for (int32 i = 0; i < HistoryList.Num(); ++i)
    {
        if (!HistoryItemClass)
        {
            continue;
        }

        UHistoryItem* ItemWidget = CreateWidget<UHistoryItem>(this, HistoryItemClass);
        if (!ItemWidget)
        {
            continue;
        }

        ItemWidget->InitItem(HistoryList[i]);
        VerticalBox->AddChildToVerticalBox(ItemWidget);

        // Spacer 추가 (마지막 항목 제외)
        if (i < HistoryList.Num() - 1)
        {
            USpacer* Spacer = NewObject<USpacer>(VerticalBox);
            Spacer->SetSize(FVector2D(1.0f, ItemSpacing));
            VerticalBox->AddChildToVerticalBox(Spacer);
        }
    }

    PRINTLOG(TEXT("[Popup_History] Refreshed %d items"), Count);
}
```

### 5.4 OnClickClear 구현

```cpp
void UPopup_History::OnClickClear()
{
    APlayerControl* PC = ULingoGameHelper::GetPlayerControl(GetWorld());
    if (!PC || !PC->GetChatHistorySystem())
    {
        return;
    }

    PC->GetChatHistorySystem()->ClearChatHistory();
    RefreshHistoryList();

    PRINTLOG(TEXT("[Popup_History] Cleared all history"));
}
```

---

## 📌 6. UHistoryItem (아이템 위젯)

### 6.1 클래스 선언

```cpp
/// @file UHistoryItem.h
/// @brief 개별 Chat History 항목을 표시하는 위젯입니다.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "NetworkData.h"
#include "UHistoryItem.generated.h"

UCLASS()
class ONEPIECE_API UHistoryItem : public UUserWidget
{
    GENERATED_BODY()

public:
    /// @brief 히스토리 항목 초기화
    UFUNCTION(BlueprintCallable)
    void InitItem(const FChatHistoryItem& Data);

protected:
    virtual void NativeConstruct() override;

protected:
    /* ----------------- Layout ----------------- */

    /** 질문 내용 */
    UPROPERTY(meta = (BindWidget))
    TObjectPtr<class UTextBlock> Txt_Question;

    /** 답변 내용 */
    UPROPERTY(meta = (BindWidget))
    TObjectPtr<class UTextBlock> Txt_Answer;

    /** 타임스탬프 */
    UPROPERTY(meta = (BindWidget))
    TObjectPtr<class UTextBlock> Txt_Timestamp;

    /** 인덱스 표시 (선택사항) */
    UPROPERTY(meta = (BindWidget))
    TObjectPtr<class UTextBlock> Txt_Index;

private:
    FChatHistoryItem HistoryData;
};
```

**파일 위치**:
- **Header**: `Source/Onepiece/MessageBox/Public/UHistoryItem.h`
- **Source**: `Source/Onepiece/MessageBox/Private/UHistoryItem.cpp`

### 6.2 InitItem 구현

```cpp
void UHistoryItem::InitItem(const FChatHistoryItem& Data)
{
    HistoryData = Data;

    if (Txt_Index)
    {
        Txt_Index->SetText(FText::FromString(FString::Printf(TEXT("#%d"), Data.Index + 1)));
    }

    if (Txt_Question)
    {
        Txt_Question->SetText(FText::FromString(Data.Question));
    }

    if (Txt_Answer)
    {
        Txt_Answer->SetText(FText::FromString(Data.Answer));
    }

    if (Txt_Timestamp)
    {
        Txt_Timestamp->SetText(FText::FromString(Data.Timestamp));
    }
}
```

---

## 📌 7. UPopupManager 통합

### 7.1 EPopupType에 History 추가

```cpp
// EPopupType.h

UENUM(Blueprintable)
enum class EPopupType : uint8
{
    // ... 기존 타입들 ...

    LevelSelect         UMETA(DisplayName = "LevelSelect"),

    // Chat History 팝업 추가
    History             UMETA(DisplayName = "History"),
};
```

### 7.2 UPopupManager에 History 클래스 등록

```cpp
// UPopupManager.cpp

#include "UPopup_History.h"

#define HISTORY_POPUP_PATH TEXT("/Game/CustomContents/UI/Widgets/WBP_PopupHistory.WBP_PopupHistory_C")

UPopupManager::UPopupManager()
{
    // ... 기존 팝업 클래스 등록 ...

    PopupClassMap.Add(EPopupType::LevelSelect, FComponentHelper::LoadClass<UPopup_LevelSelect>(LEVELSELECT_POPUP_PATH));

    // History 팝업 클래스 등록
    PopupClassMap.Add(EPopupType::History, FComponentHelper::LoadClass<UPopup_History>(HISTORY_POPUP_PATH));
}
```

**참고**: UPopupManager는 `ShowPopupAs<T>(EPopupType)` 템플릿 함수를 제공하므로 별도의 `ShowPopup_History()` 함수를 추가할 필요가 없습니다.

---

## 📌 8. APopupTestActor 통합

### 8.1 테스트 함수 추가

```cpp
// APopupTestActor.h

protected:
    /// @brief Popup_History 테스트 (H 키)
    void TestPopupHistory();
```

### 8.2 테스트 함수 구현

```cpp
// APopupTestActor.cpp

void APopupTestActor::TestPopupHistory()
{
    // ShowPopupAs 템플릿 함수를 사용하여 History 팝업 표시
    if (auto Popup = UPopupManager::ShowPopupAs<UPopup_History>(GetWorld(), EPopupType::History))
    {
        Popup->InitPopup();
    }
}
```

### 8.3 Input Binding

```cpp
// APopupTestActor.cpp (BeginPlay 또는 SetupInputComponent)

// H 키로 Popup_History 테스트
InputComponent->BindKey(EKeys::H, IE_Pressed, this, &APopupTestActor::TestPopupHistory);
```

---

## 📌 9. UI 위젯 구조

### 9.1 UPopup_History 위젯

| 위젯 이름 | 타입 | 설명 |
|-----------|------|------|
| **Txt_Title** | UTextBlock | 팝업 타이틀 ("Chat History") |
| **Btn_Close** | UTextureButton | 닫기 버튼 (우측 상단 ❌) |
| **Btn_Clear** | UImageButton | 전체 삭제 버튼 |
| **ScrollBox** | UScrollBox | 스크롤 가능한 영역 |
| **VerticalBox** | UVerticalBox | 히스토리 아이템 컨테이너 |
| **Txt_NoData** | UTextBlock | 데이터 없을 때 표시 ("No chat history available") |

### 9.2 UHistoryItem 위젯

| 위젯 이름 | 타입 | 설명 |
|-----------|------|------|
| **Txt_Index** | UTextBlock | 인덱스 표시 ("#1", "#2", ...) |
| **Txt_Question** | UTextBlock | 질문 내용 |
| **Txt_Answer** | UTextBlock | 답변 내용 |
| **Txt_Timestamp** | UTextBlock | 타임스탬프 ("2025-12-23 14:30:25") |

---

## 📌 10. 워크플로우

### 10.1 Chat 응답 저장 플로우

```
1. User → AI Chat 질문 전송
2. Server → AI 응답 수신 (FResponseChatAnswers)
3. APlayerControl::OnChatAnswerReceived()
4. ChatHistorySystem->SaveChatHistory(question, answer)
5. GConfig에 저장:
   - ChatHistory_<UserId>_Question_<Index>
   - ChatHistory_<UserId>_Answer_<Index>
   - ChatHistory_<UserId>_Timestamp_<Index>
   - ChatHistory_<UserId>_Count
```

### 10.2 History 팝업 표시 플로우

```
1. User → IA_HISTORY 입력 (또는 APopupTestActor에서 H 키)
2. APlayerControl::OnHistory()
3. PopupManager->ShowPopup_History()
4. UPopup_History::InitPopup()
5. ChatHistorySystem->LoadAllChatHistory()
6. GConfig에서 읽기:
   - ChatHistory_<UserId>_Count → N개
   - For i in 0..N-1:
     - ChatHistory_<UserId>_Question_i
     - ChatHistory_<UserId>_Answer_i
     - ChatHistory_<UserId>_Timestamp_i
7. UHistoryItem 생성 및 VerticalBox에 추가 (상단: 구 데이터, 하단: 신 데이터)
```

---

## 📌 11. 데이터 정렬 규칙

### 11.1 저장 순서
- **Index**: 0부터 시작하여 1씩 증가
- **작은 Index**: 오래된 대화
- **큰 Index**: 최근 대화

### 11.2 표시 순서 (VerticalBox)
```
┌─────────────────────────┐
│  #1 (Index 0)          │ ← 가장 오래된 대화 (상단)
│  Question: ...         │
│  Answer: ...           │
│  2025-12-23 14:25:10   │
├─────────────────────────┤
│  #2 (Index 1)          │
│  Question: ...         │
│  Answer: ...           │
│  2025-12-23 14:26:35   │
├─────────────────────────┤
│  #3 (Index 2)          │ ← 가장 최근 대화 (하단)
│  Question: ...         │
│  Answer: ...           │
│  2025-12-23 14:28:42   │
└─────────────────────────┘
```

---

## 📌 12. 체크리스트

### ✅ 데이터 구조
- [ ] `FChatHistoryItem` 구조체 추가 (NetworkData.h)

### ✅ UChatHistorySystem
- [ ] `SaveChatHistory()` 구현
- [ ] `LoadAllChatHistory()` 구현
- [ ] `ClearChatHistory()` 구현
- [ ] `GetNextIndex()` 구현
- [ ] `GetCurrentTimestamp()` 구현
- [ ] GConfig Key 생성 함수 구현

### ✅ APlayerControl
- [ ] `ChatHistorySystem` 컴포넌트 추가
- [ ] `IA_HISTORY` 입력 액션 추가
- [ ] `OnHistory()` 함수 구현
- [ ] `OnChatAnswerReceived()`에서 히스토리 저장 추가

### ✅ UPopup_History
- [ ] 헤더 파일 생성
- [ ] 소스 파일 생성
- [ ] `InitPopup()` 구현
- [ ] `RefreshHistoryList()` 구현
- [ ] `OnClickClose()` 구현
- [ ] `OnClickClear()` 구현
- [ ] BindWidget 위젯 선언

### ✅ UHistoryItem
- [ ] 헤더 파일 생성
- [ ] 소스 파일 생성
- [ ] `InitItem()` 구현
- [ ] BindWidget 위젯 선언

### ✅ EPopupType
- [ ] `History` enum 값 추가

### ✅ UPopupManager
- [ ] `UPopup_History` include 추가
- [ ] `HISTORY_POPUP_PATH` 매크로 정의
- [ ] 생성자에서 `PopupClassMap.Add(EPopupType::History, ...)` 등록

### ✅ APopupTestActor
- [ ] `TestPopupHistory()` 함수 추가 (ShowPopupAs 사용)
- [ ] H 키 바인딩 추가

### ✅ UI 블루프린트
- [ ] WBP_Popup_History 생성
- [ ] WBP_HistoryItem 생성

---

## 📌 13. 추가 기능 (선택사항)

### 13.1 히스토리 제한
- **최대 저장 개수**: 100개로 제한 (오래된 데이터 자동 삭제)

```cpp
void UChatHistorySystem::SaveChatHistory(const FString& Question, const FString& Answer)
{
    const int32 MaxHistoryCount = 100;
    const int32 CurrentCount = GetHistoryCount();

    // 최대 개수 초과 시 가장 오래된 데이터 삭제
    if (CurrentCount >= MaxHistoryCount)
    {
        RemoveOldestHistory();
    }

    // ... 저장 로직 ...
}
```

### 13.2 검색 기능
- 질문/답변 내용으로 검색
- EditableText + SearchButton 추가

### 13.3 개별 삭제
- 각 HistoryItem에 삭제 버튼 추가
- `OnClickDeleteItem(int32 Index)` 구현

---

## 📌 14. 참고 자료

### 기존 구현 참조
- `UPopup_Interview`: ScrollBox + VerticalBox 방식
- `UPopup_InterviewItem`: 아이템 위젯 구조
- `UPopup_InterviewHello`: GConfig 저장 예시

### 관련 시스템
- `UChatHistorySystem`: Chat 히스토리 관리
- `APlayerControl`: 플레이어 입력 및 컴포넌트 소유
- `UPopupManager`: 팝업 표시 관리 (ShowPopupAs 템플릿 함수)
- `ULingoGameHelper`: UserId 가져오기

### UPopupManager 사용 패턴
```cpp
// 기본 사용법: ShowPopupAs<T>(World, EPopupType)
if (auto Popup = UPopupManager::ShowPopupAs<UPopup_History>(GetWorld(), EPopupType::History))
{
    Popup->InitPopup();
}

// 또는 PopupManager 인스턴스를 먼저 가져오는 방법
if (UPopupManager* PopupMgr = UPopupManager::Get(GetWorld()))
{
    if (auto Popup = PopupMgr->ShowPopupAs<UPopup_History>(EPopupType::History))
    {
        Popup->InitPopup();
    }
}
```

---

## 📝 구현 시 주의사항

1. **UserId 유효성 체크**: 항상 `UserId > 0` 확인
2. **GConfig Flush**: 저장 후 반드시 `GConfig->Flush()` 호출
3. **Widget Null 체크**: 모든 위젯 포인터 사용 전 Null 체크
4. **Index 범위 체크**: 배열 접근 시 `IsValidIndex()` 사용
5. **타임스탬프 형식**: "YYYY-MM-DD HH:MM:SS" 형식 유지
6. **정렬 순서**: Index 순서대로 정렬 (0부터 시작)
7. **데이터 없을 때**: `Txt_NoData` 표시, `ScrollBox` 숨김

---

**문서 버전**: 1.0
**최종 수정**: 2025-12-23
**작성자**: Claude (Onepiece 프로젝트 분석 기반)
