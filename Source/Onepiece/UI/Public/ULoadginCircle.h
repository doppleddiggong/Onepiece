// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.

#pragma once

/**
 * @file ULoadginCircle.h
 * @brief ULoadginCircle 클래스를 선언합니다.
 */
#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ULoadginCircle.generated.h"

/**
 * @brief 네트워크 대기 상태를 표시하는 로딩 서클 위젯입니다.
 * @details 단순히 표시/숨김만 처리하며, 실제 표시 여부는 ULoadingCircleManager가 카운트를 통해 관리합니다.
 */
UCLASS()
class ONEPIECE_API ULoadginCircle : public UUserWidget
{
	GENERATED_BODY()

public:
	/// @brief 위젯 생성 시 초기화 작업을 수행합니다.
	virtual void NativeConstruct() override;

	/// @brief 매 프레임 상태 표시를 갱신합니다.
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	/// @brief 로딩 서클을 표시합니다.
	UFUNCTION(BlueprintCallable, Category = "Loading")
	void Show();

	/// @brief 로딩 서클을 숨깁니다.
	UFUNCTION(BlueprintCallable, Category = "Loading")
	void Hide();

	/// @brief 위젯을 Game Viewport에 추가하여 레벨 전환 시에도 유지되도록 합니다.
	/// @param ZOrder 뷰포트에서의 레이어 순서 (높을수록 위에 표시됨)
	UFUNCTION(BlueprintCallable, Category = "Loading")
	void AddToGameViewport(int32 ZOrder);

private:
	/// @brief 로딩 스피너 회전 애니메이션을 갱신합니다.
	void UpdateLoadingSpinner(float DeltaTime);

	/// @brief 위젯 가시성을 업데이트합니다.
	void UpdateVisibility(bool bShouldShow);

public:
	/// @brief 로딩 중 전체 화면을 덮는 오버레이입니다.
	UPROPERTY(meta = (BindWidgetOptional), BlueprintReadOnly, Category = "State|Network")
	class UOverlay* RootOverlay = nullptr;

	/// @brief 네트워크 대기 상태를 나타내는 로딩 스피너 이미지입니다.
	UPROPERTY(meta = (BindWidgetOptional), BlueprintReadOnly, Category = "State|Network")
	class UImage* LoadingSpinner = nullptr;

	/// @brief 로딩 스피너의 회전 속도(초당 각도)입니다.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "State|Network")
	float SpinnerRotationSpeed = 90.0f;
};
