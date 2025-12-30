// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.

#pragma once

#include "CoreMinimal.h"
#include "CompassTargetInterface.h"
#include "NetworkData.h"
#include "GameFramework/Actor.h"
#include "ADailyKiosk.generated.h"

/**
 * @brief Daily Study Kiosk
 *
 * 사용자가 상호작용하면 ChatDaily API를 통해 "오늘의 한국어 문장 5개 주세요"를 요청하고
 * 응답이 오면 Popup_DailyStudy를 띄웁니다.
 */
UCLASS()
class ONEPIECE_API ADailyKiosk : public AActor
{
	GENERATED_BODY()

public:
	ADailyKiosk();

	class UWidgetComponent* GetWidgetComp() { return WidgetComp; };

protected:
	virtual void BeginPlay() override;

private:
	// Interaction
	UFUNCTION()
	void OnInteractionTriggered(AActor* Interactor);

	UFUNCTION()
	void OnResponseDailyQuestion(FResponseChatDailys& InResponseData, bool bWasSuccessful);

	UFUNCTION()
	void OnOutlineStateChanged(bool bShouldShowOutline);
	
protected:
	// Components
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<class USceneComponent> RootSceneComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<class UStaticMeshComponent> MeshComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<class UWidgetComponent> WidgetGuideComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<class UInteractableComponent> InteractableComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<class UBoxComponent> BoxComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<class UWidgetComponent> WidgetComp;


private:
	/** 랜덤 한국어 단어 데이터 가져오기 (Fallback용) */
	TArray<FWordData> GetRandomKoreanWords(int32 Count);
};
