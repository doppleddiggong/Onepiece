// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UPopup_ReadQuestItem.generated.h"

/// @brief Read 퀘스트 선택지 엔트리 위젯
/// @details 심볼 또는 색상 선택 항목을 표시하는 위젯입니다.
UCLASS()
class ONEPIECE_API UPopup_ReadQuestItem : public UUserWidget
{
	GENERATED_BODY()

public:
	UPopup_ReadQuestItem(const FObjectInitializer& ObjectInitializer);

protected:
	virtual void NativeConstruct() override;

public:
	//--------------------------------------------------------------//
	// BindWidget Components
	//--------------------------------------------------------------//

	/// @brief 선택 버튼
	UPROPERTY(meta = (BindWidget), BlueprintReadOnly)
	class UButton* SelectButton;

	/// @brief 선택지 텍스트
	UPROPERTY(meta = (BindWidget), BlueprintReadOnly)
	class UTextBlock* ChoiceText;

	/// @brief 상태 이미지 (정답/오답 표시)
	UPROPERTY(meta = (BindWidget), BlueprintReadOnly)
	class UImage* StateImage;

	//--------------------------------------------------------------//
	// Properties
	//--------------------------------------------------------------//

protected:
	/// @brief 선택지 값
	UPROPERTY(BlueprintReadOnly, Category = "Quest")
	FString ChoiceValue;

	/// @brief 선택 상태
	UPROPERTY(BlueprintReadOnly, Category = "Quest")
	bool bIsSelected = false;

	/// @brief 오답 상태
	UPROPERTY(BlueprintReadOnly, Category = "Quest")
	bool bIsWrong = false;

	//--------------------------------------------------------------//
	// Methods
	//--------------------------------------------------------------//

public:
	/// @brief 엔트리를 초기화합니다.
	/// @param Value [in] 선택지 값
	/// @param bEnabled [in] 활성화 여부
	UFUNCTION(BlueprintCallable, Category = "Quest")
	void InitializeEntry(const FString& Value, bool bEnabled);

	/// @brief 선택 상태를 설정합니다.
	/// @param bSelected [in] 선택 여부
	UFUNCTION(BlueprintCallable, Category = "Quest")
	void SetSelected(bool bSelected);

	/// @brief 오답 상태를 설정합니다.
	/// @param bWrong [in] 오답 여부
	UFUNCTION(BlueprintCallable, Category = "Quest")
	void SetWrong(bool bWrong);

	/// @brief 선택지 값을 반환합니다.
	UFUNCTION(BlueprintPure, Category = "Quest")
	FString GetChoiceValue() const { return ChoiceValue; }

protected:
	/// @brief 버튼 클릭 콜백
	UFUNCTION()
	void OnButtonClicked();

	/// @brief UI 스타일을 업데이트합니다
	void UpdateVisualState();

	//--------------------------------------------------------------//
	// Delegates
	//--------------------------------------------------------------//

public:
	/// @brief 선택 델리게이트
	DECLARE_MULTICAST_DELEGATE_TwoParams(FOnEntrySelected, const FString& /*Value*/, UPopup_ReadQuestItem* /*Widget*/);
	FOnEntrySelected OnEntrySelected;
};
