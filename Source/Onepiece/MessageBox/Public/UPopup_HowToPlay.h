// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.

#pragma once

#include "CoreMinimal.h"
#include "UBasePopup.h"
#include "FHowToPlayPageData.h"
#include "UPopup_HowToPlay.generated.h"

/// @brief 팝업 닫힘 이벤트 델리게이트
DECLARE_DELEGATE(FOnHowToPlayClosedDelegate);

/**
 * @brief HowToPlay 팝업 위젯
 */
UCLASS()
class ONEPIECE_API UPopup_HowToPlay : public UBasePopup
{
	GENERATED_BODY()

public:
	/// @brief 팝업을 초기화합니다.
	/// @param [in] InPageTypes 표시할 페이지 타입 목록
	void InitPopup(const TArray<EHowToPlayPageType>& InPageTypes);

	/// @brief 팝업을 초기화합니다.
	/// @param [in] InPageTypes 표시할 페이지 타입 목록
	/// @param [in] InOnClosedDelegate 팝업 닫힘 시 호출될 델리게이트
	void InitPopup(const TArray<EHowToPlayPageType>& InPageTypes, FOnHowToPlayClosedDelegate InOnClosedDelegate);

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
	TArray<EHowToPlayPageType> PageTypes;

	/// @brief 팝업 닫힘 시 호출될 델리게이트
	FOnHowToPlayClosedDelegate OnClosedDelegate;
};
