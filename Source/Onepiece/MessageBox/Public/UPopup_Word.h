// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.

#pragma once

#include "CoreMinimal.h"
#include "NetworkData.h"
#include "UBasePopup.h"
#include "UPopup_Word.generated.h"

UCLASS()
class ONEPIECE_API UPopup_Word : public UBasePopup
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Quest")
	void InitPopup(const FPhonemeData& InData);

private:	
	UFUNCTION(BlueprintCallable, Category = "Close")
	void OnClickClose();

	UFUNCTION(BlueprintCallable, Category = "Close")
	void OnClickRepeat();

	UFUNCTION()
	void OnResponseListenAudio(FResponseListenAudio& ResponseData, bool bWasSuccessful);

public:
	UPROPERTY(meta = (BindWidget), BlueprintReadOnly)
	TObjectPtr<class UTextBlock> Txt_Kor;

	UPROPERTY(meta = (BindWidget), BlueprintReadOnly)
	TObjectPtr<class UTextBlock> Txt_Proneme;
	
	UPROPERTY(meta = (BindWidget), BlueprintReadOnly)
	TObjectPtr<class UTextureButton> Btn_Repeat;

	UPROPERTY(meta = (BindWidget), BlueprintReadOnly)
	TObjectPtr<class UImageButton> Btn_Ok;

	UPROPERTY(meta = (BindWidget), BlueprintReadOnly)
	TObjectPtr<class UTextureButton> Btn_Close;

private:
	FPhonemeData Data;

	/// @brief 오디오 요청 중복 방지 플래그
	bool bIsRequest = false;

	/// @brief 현재 요청 중인 오디오 텍스트 (응답 검증용)
	FString LastRequestText;
};