// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UMainWidget.generated.h"

/**
 * 
 */
UCLASS()
class ONEPIECE_API UMainWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UMainWidget(const FObjectInitializer& ObjectInitializer);

protected:
	/// @brief 위젯 초기화와 브로드캐스트 구독을 수행합니다.
	virtual void NativeConstruct() override;
	
public:
	UPROPERTY(meta = (BindWidget), BlueprintReadOnly)
	class UTextBlock* RemainPlayTimeText;
	
	UPROPERTY()
	TObjectPtr<class UBroadcastManager> BroadcastManager;
};
