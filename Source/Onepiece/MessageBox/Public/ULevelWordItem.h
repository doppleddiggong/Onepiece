// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.

#pragma once

#include "CoreMinimal.h"
#include "EWordType.h"
#include "Blueprint/UserWidget.h"
#include "ULevelWordItem.generated.h"

/**
 * 
 */
UCLASS()
class ONEPIECE_API ULevelWordItem : public UUserWidget
{
	GENERATED_BODY()
	
public:
	void InitInfo( EWordType WordType, int32 WordCode );

	/**
	 * @brief 아이템의 색상을 설정 (현재 레벨: 녹색, 하위 레벨: 회색)
	 * @param bIsCurrentLevel true면 녹색, false면 회색
	 */
	void SetItemColor(bool bIsCurrentLevel);

public:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UImage> Image_Target;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class USizeBox> SizeBox;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UScaleBox> ScaleBox;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UTextBlock> Txt_Title;
};
