// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ChatInputBox.generated.h"

/**
 * 
 */

DECLARE_MULTICAST_DELEGATE_OneParam(FOnSendClicked, FText);

UCLASS()
class ONEPIECE_API UChatInputBox : public UUserWidget
{
	GENERATED_BODY()
	
public:
	virtual void NativeConstruct() override;
	virtual FReply NativeOnPreviewKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;
	FText FlushMessage();
	
	// 외부 바인드용 Send 버튼 클릭 델리게이트
	FOnSendClicked OnSendClicked;
	
protected:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<class UMultiLineEditableTextBox> MultiLineEditableTextBox_Input;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<class UButton> Button_Send;
	
private:
	UFUNCTION()
	void HandleSendClicked();
};
