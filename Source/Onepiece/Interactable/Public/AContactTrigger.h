// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ALingoGameState.h"
#include "CompassTargetInterface.h"
#include "AContactTrigger.generated.h"

/**
 * @brief 플레이어 접촉 시 이벤트 메시지를 전송하는 트리거 액터
 * @details 멀티플레이 환경에서 플레이어가 박스 콜라이더에 접촉하면 ServerRPC를 통해 이벤트 메시지를 전송합니다.
 * 한 번 발동되면 비활성화되어 재발동되지 않습니다.
 */
UCLASS()
class ONEPIECE_API AContactTrigger : public AActor, public ICompassTargetInterface
{
	GENERATED_BODY()

public:
	AContactTrigger();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

protected:
	/// @brief 트리거 박스 Overlap 시작 이벤트 핸들러
	UFUNCTION()
	void OnTriggerBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	/// @brief 서버에서 실행되는 트리거 처리 RPC
	UFUNCTION(Server, Reliable)
	void ServerRPC_OnTrigger(AActor* TriggeringActor);

private:
	void OnTriggerScenario(EQuestType InQuestType);

	UFUNCTION()
	void OnReadResponseScenario(struct FResponseReadScenario& ResponseData, bool bWasSuccessful);

	UFUNCTION()
	void OnListenResponseScenario(struct FResponseListenScenario& ResponseData, bool bWasSuccessful);
	
protected:
	/// @brief 트리거 영역을 정의하는 박스 컴포넌트
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Trigger")
	TObjectPtr<class UBoxComponent> TriggerBox;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Trigger")
	EQuestType QuestType = EQuestType::Read;

	/// @brief 플레이어에게 전송할 이벤트 메시지 (Detail 패널에서 설정)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Trigger")
	FString EventMessage;

	/// @brief 트리거 활성화 상태 (false = 활성화, true = 비활성화)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Trigger")
	bool bIsTriggered;

	/// @brief 디버그 드로우 표시 여부
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Debug")
	bool bShowDebugBox;

	/// @brief 디버그 박스 색상
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Debug")
	FColor DebugBoxColor;

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Voice", meta=(AllowPrivateAccess="true"))
	TObjectPtr<class UVoiceConversationSystem> VoiceConversationSystem;	

protected:
	// Compass Interface
	virtual void SetCompassMarkerInto(ECompassMarkerType InMarkerType) override;
};
