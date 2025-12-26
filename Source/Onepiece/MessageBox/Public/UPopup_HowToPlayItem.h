// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.

#pragma once

#include "CoreMinimal.h"
#include "UPageScrollItem.h"
#include "FHowToPlayPageData.h"
#include "UPopup_HowToPlayItem.generated.h"

/**
 * @brief HowToPlay 팝업의 개별 페이지 위젯
 * 각 페이지는 Control, Read, Listen, Speak, Write 중 하나를 표시합니다.
 */
UCLASS()
class ONEPIECE_API UPopup_HowToPlayItem : public UPageScrollItem
{
	GENERATED_BODY()

public:
	/// @brief 페이지 데이터를 설정하고 UI를 업데이트합니다.
	/// @param [in] InPageData 페이지 데이터
	void InitPageData(const FHowToPlayPageData& InPageData);

protected:
	/// @brief 페이지 아이콘
	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	TObjectPtr<class UImage> Img_Icon;

	/// @brief 페이지 제목
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<class UTextBlock> Txt_Title;

	/// @brief 페이지 설명
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<class UTextBlock> Txt_Description;

private:
	/// @brief 현재 페이지 데이터
	UPROPERTY(BlueprintReadOnly, Category = "HowToPlayPage", meta = (AllowPrivateAccess = "true"))
	FHowToPlayPageData PageData;
};
