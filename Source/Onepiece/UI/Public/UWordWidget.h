// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"
#include "NetworkData.h"
#include "UWordWidget.generated.h"

/**
 * @brief 단어 표시 위젯
 *
 * 여러 단어의 한국어를 띄어쓰기로 나열하고, 클릭하면 해당 단어의 영어, 한국어, 발음을 표시합니다.
 */
UCLASS()
class ONEPIECE_API UWordWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

	UFUNCTION(BlueprintCallable, Category = "Word")
	void InitWordData(const FWordData& InWordData);

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class URichText> Rich_Kor;

	/** 발음 표시 텍스트 */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UTextBlock> Txt_Eng;
};