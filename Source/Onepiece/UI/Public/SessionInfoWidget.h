// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SessionInfoWidget.generated.h"

/**
 * 
 */
UCLASS()
class ONEPIECE_API USessionInfoWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;

public:
	UPROPERTY(meta=(BindWidget))
	class UTextBlock* Txt_SessionName;
	// 참여 버튼
	UPROPERTY(meta=(BindWidget))
	class UButton* Btn_Join;

	// 참여 버튼 클릭 함수
	UFUNCTION()
	void OnClickJoin();
	
public:
	// 세션 목록 중 몇 번째 있는지
	int32 sessionIdx;
	// 정보 설정 함수
	void SetSessionInfo(int32 idx, FString sessionName);
};
