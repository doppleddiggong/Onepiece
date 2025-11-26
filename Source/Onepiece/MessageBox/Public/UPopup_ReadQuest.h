// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.

#pragma once

#include "CoreMinimal.h"
#include "NetworkData.h"
#include "Blueprint/UserWidget.h"
#include "UPopup_ReadQuest.generated.h"

/// @brief Read 퀘스트 메인 위젯
/// @details Step1(Read) 퀘스트의 UI를 관리합니다.
UCLASS()
class ONEPIECE_API UPopup_ReadQuest : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;

public:
	UFUNCTION(BlueprintCallable, Category = "Quest")
	void InitPopup(const FResponseScenario& InScenarioData);

private:
	UFUNCTION(BlueprintCallable, Category = "Close")
	void OnClickClose();

public:
	UPROPERTY(meta = (BindWidget), BlueprintReadOnly)
	class UWordWidget* WordWidget;

	UPROPERTY(meta = (BindWidget), BlueprintReadOnly)
	class UTextureButton* Btn_Exit;
	
private:
	FResponseScenario ScenarioData;
};