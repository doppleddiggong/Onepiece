// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ChatInputBox.generated.h"

/**
 * 
 */


UCLASS()
class ONEPIECE_API UChatInputBox : public UUserWidget
{
	GENERATED_BODY()
	
public:
	virtual void NativeOnInitialized() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	virtual FReply NativeOnPreviewKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;

	void SetOwningChatWidget(class UChatWidget* InChatWidget) { OwningChatWidget = InChatWidget; }

	void SetInputFocus(bool bFocus);
	bool HasKeyboardFocus();

private:
	FText FlushMessage();

	bool IsAIAsk(const FString& InMessage, FString& OutQuestion) const;
	bool IsDailyAsk(const FString& InMessage, FString& OutQuestion) const;

	UFUNCTION()
	void OnDailyAnswerReceived(FResponseChatAnswers& ResponseData, bool bWasSuccessful);
	
	UFUNCTION()
	void HandleSendClicked();

public:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<class UMultiLineEditableTextBox> MultiLineEditableTextBox_Input;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<class UButton> Button_Send;
	
private:
	// 이전 프레임의 포커스 상태 추적
	bool bWasFocused = false;

	// 부모 ChatWidget 참조
	UPROPERTY()
	TObjectPtr<class UChatWidget> OwningChatWidget = nullptr;
};
