// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.

#pragma once

#include "CoreMinimal.h"
#include "EQuestRole.h"
#include "Delegates/Delegate.h"
#include "Blueprint/UserWidget.h"
#include "UQuestInfoWidget.generated.h"


struct FResponseListenResult;
struct FResponseReadResult;
struct FWordData;

UCLASS()
class ONEPIECE_API UQuestInfoWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	
public:
	UFUNCTION()
	void InitQuestInfo(EQuestRole QuestRole);

private:
	void SetQuestText(const FWordData& WordData) const;

	void OnQuestScenarioDataReceived();
	void OnReadResultReceived(const FResponseReadResult& ResponseData);
	void OnListenResultReceived(const FResponseListenResult& ResponseData);
	
public:
	UPROPERTY(meta = (BindWidget), BlueprintReadOnly, Category = "Style")
	TObjectPtr<class UTextBlock> Txt_Message;

	UPROPERTY(meta = (BindWidgetOptional), BlueprintReadOnly, Category = "Style")
	TObjectPtr<class UImage> Image_Synbol;

	FDelegateHandle QuestScenarioDataDelegateHandle;
	FDelegateHandle ReadResultDelegateHandle;
	FDelegateHandle ListenResultDelegateHandle;

	EQuestRole QuestRole;
};