// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.

#include "UPopupManager.h"
#include "UPopup_InputMsg.h"
#include "FComponentHelper.h"
#include "GameLogging.h"
#include "Onepiece/Onepiece.h"

#define POPUPMSG_PATH TEXT("/Game/CustomContents/UI/Widgets/WBP_PopupMsg.WBP_PopupMsg_C")
#define INPUT_POPUPMSG_PATH TEXT("/Game/CustomContents/UI/Widgets/WBP_InputPopupMsg.WBP_InputPopupMsg_C")

UPopupManager::UPopupManager()
{
	// 기본 팝업 클래스 등록
	PopupClassMap.Add(EPopupType::MsgBox, FComponentHelper::LoadClass<UPopup_MsgBox>(POPUPMSG_PATH));
	PopupClassMap.Add(EPopupType::InputMsg_Register, FComponentHelper::LoadClass<UPopup_InputMsg>(INPUT_POPUPMSG_PATH));
	PopupClassMap.Add(EPopupType::InputMsg_Login, FComponentHelper::LoadClass<UPopup_InputMsg>(INPUT_POPUPMSG_PATH));
}

// ========================================
// 범용 팝업 관리 함수
// ========================================

UUserWidget* UPopupManager::ShowPopup(EPopupType Type)
{
	UUserWidget* PopupWidget = EnsurePopupWidget(Type);
	if (!PopupWidget)
	{
		PRINTLOG(TEXT("[UPopupManager] Failed to create popup: %s"), *ENUM_TO_NAME(EPopupType, Type));
		return nullptr;
	}

	// 이미 뷰포트에 있으면 스택 맨 위로 이동
	if (PopupWidget->IsInViewport())
	{
		RemovePopupFromStack(Type);
	}
	else
	{
		// 뷰포트에 추가
		PopupWidget->AddToViewport(GameLayer::Popup);
	}

	// 스택에 추가
	PushPopupToStack(Type);

	return PopupWidget;
}

void UPopupManager::HidePopup(EPopupType Type, bool bDestroyWidget)
{
	UUserWidget** PopupWidgetPtr = PopupWidgetMap.Find(Type);
	if (!PopupWidgetPtr || !(*PopupWidgetPtr))
		return;

	UUserWidget* PopupWidget = *PopupWidgetPtr;

	// 뷰포트에서 제거
	if (PopupWidget->IsInViewport())
	{
		PopupWidget->RemoveFromParent();
	}

	// 스택에서 제거
	RemovePopupFromStack(Type);

	// 위젯 파괴
	if (bDestroyWidget)
	{
		PopupWidgetMap.Remove(Type);
	}
}

void UPopupManager::HideCurrentPopup(bool bDestroyWidget)
{
	if (PopupStack.Num() == 0)
		return;

	EPopupType CurrentType = PopupStack.Last();
	HidePopup(CurrentType, bDestroyWidget);
}

void UPopupManager::HideAllPopups(bool bDestroyWidgets)
{
	// 스택을 역순으로 순회하며 모두 닫기
	while (PopupStack.Num() > 0)
	{
		HideCurrentPopup(bDestroyWidgets);
	}
}

// ========================================
// 팝업 쿼리 함수
// ========================================

bool UPopupManager::GetCurrentPopupType(EPopupType& OutType) const
{
	if (PopupStack.Num() == 0)
		return false;

	OutType = PopupStack.Last();
	return true;
}

UUserWidget* UPopupManager::GetCurrentPopupWidget() const
{
	if (PopupStack.Num() == 0)
		return nullptr;

	EPopupType CurrentType = PopupStack.Last();
	return GetPopupWidget(CurrentType);
}

UUserWidget* UPopupManager::GetPopupWidget(EPopupType Type) const
{
	// 1. 해당 타입의 팝업이 맵에 있는지 확인
	if (!PopupWidgetMap.Contains(Type))
		return nullptr;

	// 2. 맵에서 위젯 가져오기
	UUserWidget* Widget = PopupWidgetMap[Type];

	// 3. 위젯이 유효한지 확인 후 반환
	return Widget;
}

bool UPopupManager::IsPopupInStack(EPopupType Type) const
{
	return PopupStack.Contains(Type);
}

int32 UPopupManager::GetPopupStackCount() const
{
	return PopupStack.Num();
}

// ========================================
// 메시지 박스 전용 함수
// ========================================

void UPopupManager::ShowMsgBox(
	const FString& InTitle,
	const FString& InDescription,
	EMsgBoxType InType,
	const FOnMsgBoxOkDelegate& InOkDelegate)
{
	UUserWidget* Widget = ShowPopup(EPopupType::MsgBox);
	if (!Widget)
		return;

	if (auto MsgBox = Cast<UPopup_MsgBox>(Widget))
	{
		MsgBox->InitPopup(InTitle, InDescription, InType, InOkDelegate);
	}
}

void UPopupManager::ShowMsgBox(
	const FString& InTitle,
	const FString& InDescription,
	EMsgBoxType InType,
	const FOnMsgBoxOkDelegate& InOkDelegate,
	const FOnMsgBoxCancelDelegate& InCancelDelegate)
{
	UUserWidget* Widget = ShowPopup(EPopupType::MsgBox);
	if (!Widget)
		return;

	if (auto Popup = Cast<UPopup_MsgBox>(Widget))
	{
		Popup->InitPopup(InTitle, InDescription, InType, InOkDelegate, InCancelDelegate);
	}
}

void UPopupManager::ShowMsgBoxSimple(
	const FString& InTitle,
	const FString& InDescription,
	EMsgBoxType InType)
{
	// 델리게이트 없이 팝업만 표시 (블루프린트용)
	UUserWidget* Widget = ShowPopup(EPopupType::MsgBox);
	if (!Widget)
		return;

	if (auto MsgBox = Cast<UPopup_MsgBox>(Widget))
	{
		// 빈 델리게이트로 초기화
		FOnMsgBoxOkDelegate EmptyOkDelegate;
		FOnMsgBoxCancelDelegate EmptyCancelDelegate;
		MsgBox->InitPopup(InTitle, InDescription, InType, EmptyOkDelegate, EmptyCancelDelegate);
	}
}

// ========================================
// 내부 헬퍼 함수
// ========================================

UUserWidget* UPopupManager::EnsurePopupWidget(EPopupType Type)
{
	UWorld* World = GetWorld();
	if (!World || !World->IsGameWorld())
		return nullptr;

	// 이미 생성된 위젯이 있는지 확인
	UUserWidget** ExistingWidgetPtr = PopupWidgetMap.Find(Type);
	if (ExistingWidgetPtr && IsValid(*ExistingWidgetPtr))
	{
		UUserWidget* ExistingWidget = *ExistingWidgetPtr;
		// 같은 월드인지 확인
		if (ExistingWidget->GetWorld() == World)
		{
			return ExistingWidget;
		}
		else
		{
			// 다른 월드면 제거
			if (ExistingWidget->IsInViewport())
				ExistingWidget->RemoveFromParent();
			PopupWidgetMap.Remove(Type);
		}
	}

	// 팝업 클래스 가져오기
	TSubclassOf<UUserWidget>* PopupClassPtr = PopupClassMap.Find(Type);
	if (!PopupClassPtr || !(*PopupClassPtr))
	{
		PRINTLOG(TEXT("[UPopupManager] No popup class registered for type: %s"),
			*ENUM_TO_NAME(EPopupType, Type));
		return nullptr;
	}

	// 플레이어 컨트롤러 가져오기
	ULocalPlayer* LocalPlayer = GetLocalPlayer();
	if (!LocalPlayer)
		return nullptr;

	APlayerController* PC = LocalPlayer->GetPlayerController(World);
	if (!PC)
		return nullptr;

	// 위젯 생성
	UUserWidget* NewWidget = CreateWidget<UUserWidget>(PC, *PopupClassPtr);
	if (!NewWidget)
	{
		PRINTLOG(TEXT("[UPopupManager] Failed to create widget for type: %s"),
			*ENUM_TO_NAME(EPopupType, Type));
		return nullptr;
	}

	// 맵에 추가
	PopupWidgetMap.Add(Type, NewWidget);

	return NewWidget;
}

void UPopupManager::PushPopupToStack(EPopupType Type)
{
	// 이미 스택에 있으면 제거 후 재추가 (최상단으로)
	RemovePopupFromStack(Type);
	PopupStack.Add(Type);
}

void UPopupManager::RemovePopupFromStack(EPopupType Type)
{
	PopupStack.Remove(Type);
}