// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UPageScrollDot.generated.h"

/**
 * @brief 페이지 스크롤 뷰의 인디케이터 위젯
 * 현재 페이지를 시각적으로 표시하는 도트 인디케이터입니다.
 */
UCLASS()
class ONEPIECE_API UPageScrollDot : public UUserWidget
{
	GENERATED_BODY()

public:
	/// @brief 도트 개수를 설정합니다.
	/// @param [in] PageCount 페이지 수
	UFUNCTION(BlueprintCallable, Category = "PageScrollDot")
	void SetNumberOfPages(int32 PageCount);

	/// @brief 현재 선택된 페이지를 설정합니다.
	/// @param [in] PageIndex 페이지 인덱스
	UFUNCTION(BlueprintCallable, Category = "PageScrollDot")
	void SetCurrentPage(int32 PageIndex);

private:
	/// @brief 도트 위젯의 Image 컴포넌트를 찾습니다.
	/// @param [in] DotWidget 도트 위젯
	/// @return Image 컴포넌트
	class UImage* GetDotImage(UUserWidget* DotWidget) const;

	/// @brief 도트의 색상과 스케일을 설정합니다.
	/// @param [in] DotWidget 도트 위젯
	/// @param [in] Color 색상
	/// @param [in] Scale 스케일
	void UpdateDotAppearance(UUserWidget* DotWidget, FLinearColor Color, float Scale);
	
public:
	//~ 데이터 설정

	/// @brief 개별 도트 위젯 클래스
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PageScrollDot|Data")
	TSubclassOf<class UUserWidget> DotWidgetClass;

	//~ 외형 설정

	/// @brief 선택된 도트의 색상
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PageScrollDot|Appearance")
	FLinearColor SelectedColor = FLinearColor::White;

	/// @brief 비선택된 도트의 색상
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PageScrollDot|Appearance")
	FLinearColor UnselectedColor = FLinearColor(0.5f, 0.5f, 0.5f, 1.0f);

	/// @brief 선택된 도트의 스케일
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PageScrollDot|Appearance")
	float SelectedScale = 1.2f;

	/// @brief 비선택된 도트의 스케일
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PageScrollDot|Appearance")
	float UnselectedScale = 1.0f;

	//~ 애니메이션 설정

	/// @brief 도트 애니메이션 속도 (초 단위)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PageScrollDot|Animation")
	float AnimationSpeed = 0.15f;

	//~ BindWidget 컴포넌트

	/// @brief 생성된 도트들을 담을 컨테이너
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<class UHorizontalBox> DotContainer;

private:
	/// @brief 생성된 도트 위젯 목록
	UPROPERTY()
	TArray<TObjectPtr<UUserWidget>> dotList;

	/// @brief 현재 선택된 페이지 인덱스
	int32 currentPageIndex = 0;
};
