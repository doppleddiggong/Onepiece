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
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	UFUNCTION()
	void SendMessage(FResponseUserMe sendUser, FText inMessage, int32 PlayerIndex);

	void FocusInput();
	void OnInputFocusChanged(bool bHasFocus);
	
protected:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<class UVerticalBox> VerticalBox_Content;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<class USizeBox> SizeBox_Chat;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<class UScrollBox> ScrollBox_ChatBox;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<class USpacer> Spacer_Content;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<class UChatInputBox> ChatInputBox;
	
protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<class UChatBoxWidget> LeftChatBoxWidgetClass;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<class UChatBoxWidget> RightChatBoxWidgetClass;
	
private:
	class UChatBoxWidget* CreateChatBox(bool bIsSender);
	void StartFadeOutTimer();
	void OnFadeOutTimerComplete();

	// 페이드 아웃 타이머
	FTimerHandle FadeOutTimerHandle;

	// 페이드 상태
	bool bIsFading = false;
	float CurrentOpacity = 1.0f;
	float TargetOpacity = 1.0f;

	// 페이드 속도 (초당 변화량)
	float FadeSpeed = 2.0f;

	// 자동 숨김 대기 시간
	float AutoHideDelay = 5.0f;
};
