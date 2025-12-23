// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.

/// @file UPopup_History.h
/// @brief Chat History를 표시하는 팝업 위젯입니다.

#pragma once

#include "CoreMinimal.h"
#include "UBasePopup.h"
#include "NetworkData.h"
#include "UPopup_History.generated.h"

/// @brief Chat History 팝업
/// @details ScrollBox + VerticalBox 구조로 히스토리 아이템을 표시합니다.
UCLASS()
class ONEPIECE_API UPopup_History : public UBasePopup
{
	GENERATED_BODY()

public:
	/// @brief 팝업 초기화
	UFUNCTION(BlueprintCallable)
	void InitPopup();

protected:
	virtual void NativeConstruct() override;

private:
	/// @brief 히스토리 아이템을 생성하여 VerticalBox에 추가합니다.
	void RefreshHistoryList();

	UFUNCTION()
	void OnClickClose();

	UFUNCTION()
	void OnClickClear();

protected:
	/* ------------------- Layout ------------------- */

	/// @brief 팝업 타이틀
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UTextBlock> Txt_Title;

	/// @brief 닫기 버튼
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UTextureButton> Btn_Close;

	/// @brief 전체 삭제 버튼
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UImageButton> Btn_Clear;

	/// @brief 스크롤 가능한 히스토리 패널
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UScrollBox> ScrollBox;

	/// @brief 히스토리 항목들이 추가될 VerticalBox
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UVerticalBox> VerticalBox;

	/// @brief 데이터가 없을 때 표시할 텍스트
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UTextBlock> Txt_NoData;

	/* ------------------- Classes ------------------- */

	/// @brief 히스토리 항목 위젯 클래스
	UPROPERTY(EditDefaultsOnly, Category="Popup")
	TSubclassOf<class UHistoryItem> HistoryItemClass;

	/* ------------------- Settings ------------------- */

	/// @brief 히스토리 항목 간 간격 (Spacer Height)
	UPROPERTY(EditDefaultsOnly, Category="Popup", meta=(ClampMin="0.0", ClampMax="200.0"))
	float ItemSpacing = 15.0f;
};
