// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ChatWidget.generated.h"

/**
 * 
 */
UCLASS()
class ONEPIECE_API UChatWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UChatWidget(const FObjectInitializer& ObjectInitializer);
	virtual void NativeConstruct() override;
	
	UFUNCTION()
	void SendMessage(FText inMessage);
	
protected:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<class UBorder> Border_BG;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<class UVerticalBox> VerticalBox_Content;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<class USizeBox> SizeBox_Chat;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<class UScrollBox> ScrollBox_ChatBox;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<class UChatBoxWidget> ChatMessage;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<class USpacer> Spacer_Content;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<class UChatInputBox> ChatInputBox;
	
protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<class UChatBoxWidget> ChatBoxWidgetClass;
	
};
