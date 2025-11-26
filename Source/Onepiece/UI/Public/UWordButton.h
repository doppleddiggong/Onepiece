// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "UWordButton.generated.h"

/**
 * @brief 단어 버튼 위젯
 *
 * 클릭 가능한 단어 버튼으로, 인덱스 정보를 포함합니다.
 */
UCLASS()
class ONEPIECE_API UWordButton : public UUserWidget
{
	GENERATED_BODY()

public:
	// ~ Begin UUserWidget Interface
	virtual void NativeConstruct() override;
	// ~ End UUserWidget Interface

	/**
	 * @brief 버튼 초기화
	 * @param InText 버튼에 표시할 텍스트
	 * @param InIndex 버튼의 인덱스
	 */
	void InitializeWordButton(const FString& InText, int32 InIndex);

	/**
	 * @brief 버튼 인덱스 가져오기
	 */
	int32 GetIndex() const { return Index; }

protected:
	/** 버튼 위젯 */
	UPROPERTY(meta = (BindWidget))
	UButton* Btn_Word;

	/** 텍스트 위젯 */
	UPROPERTY(meta = (BindWidget))
	UTextBlock* Txt_Word;

	/** 버튼 인덱스 */
	int32 Index = 0;

	/**
	 * @brief 버튼 클릭 핸들러
	 */
	UFUNCTION()
	void OnButtonClicked();

public:
	/**
	 * @brief 버튼 클릭 델리게이트
	 */
	DECLARE_MULTICAST_DELEGATE_OneParam(FOnWordButtonClicked, int32 /*Index*/);
	FOnWordButtonClicked OnWordButtonClicked;
};
