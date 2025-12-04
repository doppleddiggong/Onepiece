// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UTutorMessage.generated.h"

UCLASS()
class ONEPIECE_API UTutorMessage : public UUserWidget
{
	GENERATED_BODY()

public:
	/// @brief 메시지 텍스트를 설정합니다.
	/// @param NewMessage 표시할 메시지
	UFUNCTION(BlueprintCallable, Category = "Tutor")
	void SetMessageText(const FText& NewMessage);

public:
	/// @brief 메시지 텍스트 블록
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UTextBlock> Txt_Message;
};