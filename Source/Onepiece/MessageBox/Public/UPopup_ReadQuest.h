// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.

#pragma once

#include "CoreMinimal.h"
#include "ALingoGameState.h"
#include "EQuestRole.h"
#include "NetworkData.h"
#include "UBasePopup.h"
#include "Blueprint/UserWidget.h"
#include "UPopup_ReadQuest.generated.h"

/// @brief Read 퀘스트 메인 위젯
/// @details Step1(Read) 퀘스트의 UI를 관리합니다.
UCLASS()
class ONEPIECE_API UPopup_ReadQuest : public UBasePopup
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Quest")
	void InitRead(const FResponseReadScenario& InScenarioData);

	UFUNCTION(BlueprintCallable, Category = "Quest")
	void InitListen(const FResponseListenScenario& InScenarioData);

	
private:
	UFUNCTION()
	void InitQuestInfo(EQuestRole QuestRole);

	UFUNCTION(BlueprintCallable, Category = "Close")
	void OnClickClose();

	void ListenAudio(const FString& AudioText);
	void OnResponseListenAudio(FResponseListenAudio& ResponseData, bool bWasSuccessful);

	
public:
	UPROPERTY(meta = (BindWidget), BlueprintReadOnly)
	TObjectPtr<class UWordWidget> WordWidget;

	UPROPERTY(meta = (BindWidget), BlueprintReadOnly)
	TObjectPtr<class UTextureButton>Btn_Exit;
	
private:
	FResponseReadScenario ReadData;
	FResponseListenScenario ListenData;
	EQuestType QuestType = EQuestType::Read;

	bool bIsRequest = false;
};