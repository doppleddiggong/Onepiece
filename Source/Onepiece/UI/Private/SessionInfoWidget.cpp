// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.


#include "SessionInfoWidget.h"

#include "UDialogManager.h"
#include "UHoverButton.h"
#include "ULingoGameInstance.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"

void USessionInfoWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// 중복 바인딩 방지
	if (Btn_Join)
	{
		Btn_Join->OnButtonClickedEvent.RemoveDynamic(this, &USessionInfoWidget::OnClickJoin);
		Btn_Join->OnButtonClickedEvent.AddDynamic(this, &USessionInfoWidget::OnClickJoin);
	}
}

void USessionInfoWidget::OnClickJoin()
{
	// Game Instance 가져오자
	ULingoGameInstance* gi = Cast<ULingoGameInstance>(GetGameInstance());
	// sessionIdx 번째 세션에 참여
	gi->JoinOtherSession(sessionIdx);

	UDialogManager::Get(GetWorld())->ShowToast(TEXT("Session Join Success"));
}

void USessionInfoWidget::SetSessionInfo(int32 idx, FString sessionName)
{
	// 세션 순서 설정
	sessionIdx = idx;
	// 세션 이름 설정
	Txt_SessionName->SetText(FText::FromString(sessionName));
}
