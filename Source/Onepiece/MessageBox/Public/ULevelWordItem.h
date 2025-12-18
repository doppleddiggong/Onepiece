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

protected:
	/**
	 * @brief 긴 텍스트를 축약하여 반환 (방법 2: 단순 절삭)
	 * @param OriginalText 원본 텍스트
	 * @param MaxLength 최대 길이
	 * @return 축약된 텍스트
	 */
	FString TruncateText(const FString& OriginalText, int32 MaxLength = 12);

	/**
	 * @brief ScaleBox를 사용한 텍스트 크기 자동 조절 (방법 1)
	 */
	void ApplyScaleBoxMode();

	/**
	 * @brief 텍스트 절삭 방식 적용 (방법 2)
	 * @param Text 표시할 텍스트
	 */
	void ApplyTruncateMode(const FString& Text);

public:
	/** 텍스트 처리 방식 (true: ScaleBox 사용, false: 단순 절삭) */
	UPROPERTY(EditAnywhere, Category = "LevelWordItem")
	bool bUseScaleBox = false;

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
