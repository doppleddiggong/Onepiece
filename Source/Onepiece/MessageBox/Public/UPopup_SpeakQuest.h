// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.

#pragma once

#include "CoreMinimal.h"
#include "NetworkData.h"
#include "UBasePopup.h"
#include "UPopup_SpeakQuest.generated.h"

/**
 * 
 */
UCLASS()
class ONEPIECE_API UPopup_SpeakQuest : public UBasePopup
{
	GENERATED_BODY()

public:
	void InitPopup(const FOnMsgBoxOkDelegate& InOkDelegate);
	void OnResponseListenAudio(FResponseListenAudio& ResponseListenAudio, bool bArg);
	void RequestListenAudio(const FString& AudioText);

private:
	UFUNCTION()
	void OnClickOk();
	
public:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UImageButton> Btn_Ok;

private:
	FOnMsgBoxOkDelegate OnOkDelegate;
	
	bool bIsRequest;
};
