// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.

/// @file UHistoryItem.h
/// @brief 개별 Chat History 항목을 표시하는 위젯입니다.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "FChatHistoryItem.h"
#include "UHistoryItem.generated.h"

/// @brief Chat History 개별 아이템 위젯
/// @details 질문, 답변, 타임스탬프, 인덱스를 표시합니다.
UCLASS()
class ONEPIECE_API UHistoryItem : public UUserWidget
{
	GENERATED_BODY()

public:
	/// @brief 히스토리 항목 초기화
	/// @param Data [in] 히스토리 아이템 데이터
	UFUNCTION(BlueprintCallable)
	void InitItem(const FChatHistoryItem& Data);

protected:
	virtual void NativeConstruct() override;

protected:
	/* ----------------- Layout ----------------- */

	/// @brief 질문 내용
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UTextBlock> Txt_Question;

	/// @brief 답변 내용
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UTextBlock> Txt_Answer;

	/// @brief 타임스탬프
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UTextBlock> Txt_Timestamp;

private:
	FChatHistoryItem HistoryData;
};
