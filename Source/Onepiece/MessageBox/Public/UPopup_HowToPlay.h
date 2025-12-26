// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.

#pragma once

#include "CoreMinimal.h"
#include "UBasePopup.h"
#include "FHowToPlayPageData.h"
#include "UPopup_HowToPlay.generated.h"

/**
 * @brief HowToPlay 팝업 위젯
 * @details Control, Read, Listen, Speak, Write 5개 페이지로 구성된 게임 가이드 팝업
 */
UCLASS()
class ONEPIECE_API UPopup_HowToPlay : public UBasePopup
{
	GENERATED_BODY()

public:
	/// @brief 팝업을 초기화합니다.
	UFUNCTION(BlueprintCallable, Category = "PopupHowToPlay")
	void InitPopup();

private:
	void InitPageScrollView();

	/// @brief 페이지 네비게이션 버튼 상태를 업데이트합니다.
	void UpdateNavigationButtons();

	/// @brief 페이지 데이터 배열을 가져옵니다.
	/// @return 페이지 데이터 배열
	TArray<FHowToPlayPageData> GetPageDataArray() const;
	
	/// @brief 닫기 버튼 클릭 이벤트
	UFUNCTION()
	void OnClickClose();

	/// @brief 이전 페이지 버튼 클릭 이벤트
	UFUNCTION()
	void OnClickPrev();

	/// @brief 다음 페이지 버튼 클릭 이벤트
	UFUNCTION()
	void OnClickNext();

	/// @brief 페이지 변경 이벤트
	UFUNCTION()
	void OnPageChanged(int32 PrevPage, int32 CurrentPage);
	
protected:
	/// @brief 페이지 스크롤 뷰
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<class UPageScrollView> PageScrollView;

	/// @brief 닫기 버튼 (우측 상단)
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UTextureButton> Btn_Close;

	/// @brief 이전 페이지 버튼 (선택적)
	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	TObjectPtr<class UTextureButton> Btn_Prev;

	/// @brief 다음 페이지 버튼 (선택적)
	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	TObjectPtr<class UTextureButton> Btn_Next;

private:
	TArray<EHowToPlayPageType> pageTypes = {
		EHowToPlayPageType::Control,
		EHowToPlayPageType::Read,
		EHowToPlayPageType::Listen,
		EHowToPlayPageType::Speak,
		EHowToPlayPageType::Write
	};
};
