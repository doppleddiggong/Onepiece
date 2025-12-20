// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.


#include "ChatWidget.h"

#include "APlayerControl.h"
#include "ChatBoxWidget.h"
#include "ChatInputBox.h"
#include "GameLogging.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Components/MultiLineEditableTextBox.h"
#include "Components/ScrollBox.h"
#include "Components/ScrollBoxSlot.h"
#include "Components/VerticalBox.h"
#include "GameFramework/GameStateBase.h"

UChatWidget::UChatWidget(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	ConstructorHelpers::FClassFinder<UChatBoxWidget> LeftChatBoxWidgetClassRef(TEXT("/Game/CustomContents/UI/Widgets/Chat/WBP_LeftChatMessage.WBP_LeftChatMessage_C"));
	if (LeftChatBoxWidgetClassRef.Succeeded())
	{
		LeftChatBoxWidgetClass = LeftChatBoxWidgetClassRef.Class;
	}
	ConstructorHelpers::FClassFinder<UChatBoxWidget> RightChatBoxWidgetClassRef(TEXT("/Game/CustomContents/UI/Widgets/Chat/WBP_RightChatMessage.WBP_RightChatMessage_C"));
	if (RightChatBoxWidgetClassRef.Succeeded())
	{
		RightChatBoxWidgetClass = RightChatBoxWidgetClassRef.Class;
	}
}

void UChatWidget::NativeConstruct()
{
	Super::NativeConstruct();

	SetIsFocusable(true);

	// 초기 상태는 페이드아웃 (약간 투명하게)
	SetRenderOpacity(0.2f);
	CurrentOpacity = 0.2f;
	TargetOpacity = 0.2f;

	// ChatInputBox에 자신을 설정
	if (ChatInputBox)
	{
		ChatInputBox->SetOwningChatWidget(this);
	}
}

void UChatWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	// 페이드 처리
	if (bIsFading && CurrentOpacity != TargetOpacity)
	{
		// 부드럽게 Opacity 변경
		float Delta = FadeSpeed * InDeltaTime;
		if (CurrentOpacity > TargetOpacity)
		{
			CurrentOpacity = FMath::Max(CurrentOpacity - Delta, TargetOpacity);
		}
		else
		{
			CurrentOpacity = FMath::Min(CurrentOpacity + Delta, TargetOpacity);
		}

		SetRenderOpacity(CurrentOpacity);

		// 목표 도달 시 페이드 종료
		if (FMath::IsNearlyEqual(CurrentOpacity, TargetOpacity, 0.01f))
		{
			bIsFading = false;
		}
	}
}

void UChatWidget::SendMessage(FResponseUserMe sendUser, FText inMessage, int32 PlayerIndex)
{
	auto* PC = Cast<APlayerControl>(GetWorld()->GetFirstPlayerController<APlayerController>());
	if (!PC)
	{
		PRINTLOG(TEXT("Cannot get playercontroller"));
	}

	// 메시지 받은 플레이어 정보 가져오기
	FResponseUserMe info = PC->GetUserInfo();
	bool bIsSender = (info.id == sendUser.id);

	// ChatBox 생성
	UChatBoxWidget* newChat = CreateChatBox(bIsSender);
	newChat->SetMessage(inMessage);

	newChat->SetPlayerProfile(
		sendUser.GetChatProfileBg(PlayerIndex),
		sendUser.GetChatProfileTextureType(PlayerIndex));

	newChat->SetPlayerName(FText::FromString(sendUser.username));
	newChat->SetMessage(inMessage);
	newChat->SetChatBubbleColor(bIsSender);

	// ChatBox에 추가 & 정렬
	VerticalBox_Content->AddChild(newChat);

	UScrollBoxSlot* parentSlot = Cast<UScrollBoxSlot>(newChat->Slot);
	if (parentSlot)
	{
		parentSlot->SetHorizontalAlignment((bIsSender ? HAlign_Left : HAlign_Right));
	}

	// 위젯의 레이아웃을 강제로 갱신하여 정확한 크기 계산
	newChat->ForceLayoutPrepass();

	// 부모 위젯들의 레이아웃을 무효화하여 다음 틱에 재계산
	VerticalBox_Content->InvalidateLayoutAndVolatility();
	ScrollBox_ChatBox->InvalidateLayoutAndVolatility();

	// 현재 스크롤 값 & 마지막 스크롤 값
	float scrollOffset = ScrollBox_ChatBox->GetScrollOffset();
	float scrollOffsetOfEnd = ScrollBox_ChatBox->GetScrollOffsetOfEnd();
	
	if (bIsSender || scrollOffset == scrollOffsetOfEnd)
	{
		// 레이아웃 갱신 후 스크롤 (타이머 사용)
		FTimerHandle ScrollTimerHandle;
		GetWorld()->GetTimerManager().SetTimer(ScrollTimerHandle, [this, newChat]()
		{
			// 마지막 위젯을 뷰에 표시
			ScrollBox_ChatBox->ScrollWidgetIntoView(newChat, true, EDescendantScrollDestination::BottomOrRight);

			// 추가로 ScrollToEnd도 실행
			ScrollBox_ChatBox->ScrollToEnd();
		}, 0.1f, false);
	}

	UWidgetBlueprintLibrary::SetFocusToGameViewport();
	PC->SetInputMode(FInputModeGameOnly());
	PC->SetShowMouseCursor(false);

	// 채팅창을 완전히 보이게 하고 5초 후 페이드아웃 타이머 시작
	TargetOpacity = 1.0f;
	CurrentOpacity = 1.0f;
	SetRenderOpacity(1.0f);
	bIsFading = false;
	
	StartFadeOutTimer();
}

void UChatWidget::FocusInput()
{
	ChatInputBox->SetInputFocus(true);

	// 입력 포커스를 얻을 때 완전히 보이게 하고 타이머 중단
	TargetOpacity = 1.0f;
	CurrentOpacity = 1.0f;
	SetRenderOpacity(1.0f);
	bIsFading = false;

	// 타이머 중단
	if (GetWorld() && GetWorld()->GetTimerManager().IsTimerActive(FadeOutTimerHandle))
	{
		GetWorld()->GetTimerManager().ClearTimer(FadeOutTimerHandle);
	}
}

void UChatWidget::OnInputFocusChanged(bool bHasFocus)
{
	if (bHasFocus)
	{
		// 포커스를 얻으면 완전히 보이게 하고 타이머 중단
		TargetOpacity = 1.0f;
		CurrentOpacity = 1.0f;
		SetRenderOpacity(1.0f);
		bIsFading = false;

		if (GetWorld() && GetWorld()->GetTimerManager().IsTimerActive(FadeOutTimerHandle))
		{
			GetWorld()->GetTimerManager().ClearTimer(FadeOutTimerHandle);
		}
	}
	else
	{
		// 포커스를 잃으면 5초 후 페이드아웃 시작
		StartFadeOutTimer();
	}
}

void UChatWidget::StartFadeOutTimer()
{
	if (!GetWorld())
		return;

	// 기존 타이머가 있으면 취소
	if (GetWorld()->GetTimerManager().IsTimerActive(FadeOutTimerHandle))
	{
		GetWorld()->GetTimerManager().ClearTimer(FadeOutTimerHandle);
	}

	// 5초 후 페이드아웃 시작
	GetWorld()->GetTimerManager().SetTimer(FadeOutTimerHandle, this, &UChatWidget::OnFadeOutTimerComplete, AutoHideDelay, false);
}

void UChatWidget::OnFadeOutTimerComplete()
{
	// 입력창에 포커스가 있으면 페이드아웃하지 않음
	if (ChatInputBox && ChatInputBox->HasKeyboardFocus())
		return;

	// 페이드아웃 시작
	TargetOpacity = 0.2f; // 완전히 투명하게 하지 않고 약간 보이게
	bIsFading = true;
}

UChatBoxWidget* UChatWidget::CreateChatBox(bool bIsSender)
{
	return CreateWidget<UChatBoxWidget>(GetWorld(), bIsSender? LeftChatBoxWidgetClass : RightChatBoxWidgetClass );
}

