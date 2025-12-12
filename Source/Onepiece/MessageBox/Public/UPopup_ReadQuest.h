// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.

#pragma once

#include "CoreMinimal.h"
#include "ALingoGameState.h"
#include "EQuestRole.h"
#include "NetworkData.h"
#include "UBasePopup.h"
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
	FString ConvertWordDataToRichText(const FWordData& WordData);

protected:
	virtual void NativeConstruct() override;

private:
	UFUNCTION()
	void InitQuestInfo(EQuestRole QuestRole);
	UFUNCTION()
	void InitWordList(EQuestRole QuestRole);

	UFUNCTION(BlueprintCallable, Category = "Close")
	void OnClickClose();

	UFUNCTION()
	void OnClickHyperlink(const FString& LinkID, const FString& Content);

	void RequestListenAudio(const FString& AudioText);
	void OnResponseListenAudio(FResponseListenAudio& ResponseData, bool bWasSuccessful);
	
protected:
	UPROPERTY(meta = (BindWidget), BlueprintReadOnly)
	TObjectPtr<class URichTextBlock> Rich_Text;

	UPROPERTY(meta = (BindWidget), BlueprintReadOnly)
	TObjectPtr<class UTextBlock> Txt_SubTitle;
	
	UPROPERTY(meta = (BindWidget), BlueprintReadOnly)
	TObjectPtr<class UImageButton> Btn_Confirm;

	UPROPERTY(meta = (BindWidget), BlueprintReadOnly)
	TObjectPtr<class UHorizontalBox> WordBox;

	
private:
	TArray<FPhonemeData> CachedPhonemeData;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<class UWordItem> WordItemClass;
	
	FResponseReadScenario ReadData;
	EQuestType QuestType = EQuestType::Read;

	bool bIsRequest = false;
};