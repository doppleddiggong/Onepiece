// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.

#pragma once

#include "CoreMinimal.h"
#include "FResourceTextureData.h"
#include "Blueprint/UserWidget.h"
#include "ChatBoxWidget.generated.h"

/**
 * 
 */
UCLASS()
class ONEPIECE_API UChatBoxWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:	
	void SetPlayerName(FText InPlayerName) const;
	void SetMessage(FText InMessage) const;

	void SetPlayerProfile(
		FLinearColor InBgColor,
		EResourceTextureType InProfileType);

	void SetChatBubbleColor(bool IsMine) const;

	
protected:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<class UBorder> Border_ProfileBG;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<class UImage> Image_Profile;

	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<class UImage> Image_ChatTail;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<class UBorder> Border_ChatBubble;

	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<class UTextBlock> Txt_Name;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<class UTextBlock> Txt_Message;
};