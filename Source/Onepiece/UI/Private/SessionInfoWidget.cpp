// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.


#include "SessionInfoWidget.h"

#include "ULingoGameInstance.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"

void USessionInfoWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// 참여 버튼 클릭시 호출되는 함수 등록
	Btn_Join->OnClicked.AddDynamic(this, &USessionInfoWidget::OnClickJoin);
}

void USessionInfoWidget::OnClickJoin()
{
	// Game Instance 가져오자
	ULingoGameInstance* gi = Cast<ULingoGameInstance>(GetGameInstance());
	// sessionIdx 번째 세션에 참여
	gi->JoinOtherSession(sessionIdx);
}

void USessionInfoWidget::SetSessionInfo(int32 idx, FString sessionName)
{
	// 세션 순서 설정
	sessionIdx = idx;
	// 세션 이름 설정
	Txt_SessionName->SetText(FText::FromString(sessionName));
}
