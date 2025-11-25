// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "luggage.generated.h"

/// @brief 상호작용 가능한 수하물 액터
/// @details 플레이어가 선택할 수 있는 수하물 오브젝트입니다.
///          Read 퀘스트에서는 Symbol과 Color 정보를 추가로 가지며, 정답 판정에 사용됩니다.
UCLASS()
class ONEPIECE_API Aluggage : public AActor
{
	GENERATED_BODY()

public:
	Aluggage();

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	
public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	TObjectPtr<class UStaticMeshComponent> Mesh;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	TObjectPtr<class UStaticMeshComponent> Mesh1Comp;
	
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<class UInteractableComponent> InteractableComp;
	
public:
	// Color
	UPROPERTY(ReplicatedUsing=OnRep_ColorIndex)
	int32 ColorIndex = -1;

	UFUNCTION()
	void OnRep_ColorIndex();

	void ApplyColorToMesh(int32 InColorIdx);

	// Pattern
	UPROPERTY()
	FString PatternName;

	void ApplyPatternToMesh(FString InPattern);
	
public:
	//--------------------------------------------------------------//
	// Read Quest Data
	//--------------------------------------------------------------//

	/// @brief 캐리어의 심볼 (문제1 정답)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest")
	FString Symbol;

	/// @brief 캐리어의 색상 (문제2 정답)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest")
	FString Color;

	/// @brief 플레이어가 캐리어를 선택했을 때 호출됩니다.
	/// @param Interactor [in] 상호작용을 시도한 액터 (플레이어)
	UFUNCTION(BlueprintCallable, Category = "Interaction")
	void OnInteract(AActor* Interactor);

	/// @brief 서버에 캐리어 선택을 알립니다.
	/// @param Player [in] 선택한 플레이어의 PlayerState
	UFUNCTION(Server, Reliable, WithValidation)
	void ServerNotifySelection(class APlayerState* Player);
};
