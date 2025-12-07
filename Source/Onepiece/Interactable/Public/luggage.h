// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.

#pragma once

#include "CoreMinimal.h"
#include "NetworkData.h"
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
	TObjectPtr<class UBoxComponent> BoxComp;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	TObjectPtr<class UStaticMeshComponent> Mesh1Comp;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	TObjectPtr<class UStaticMeshComponent> Mesh2Comp;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	TObjectPtr<class UStaticMeshComponent> Mesh3Comp;
	
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<class UInteractableComponent> InteractableComp;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	TObjectPtr<class UWidgetComponent> WidgetComp;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	TObjectPtr<class UHookComponent> HookComp;

	//--------------------------------------------------------------//
	// Hook State (for preventing duplicate hooks)
	//--------------------------------------------------------------//

	/**
	 * @brief 훅 중 플래그 (복제됨)
	 * @details [문제] 기존에는 여러 플레이어가 동시에 같은 Luggage를 훅할 수 있었음
	 *          [해결] bIsBeingHooked 플래그로 중복 훅 방지
	 */
	UPROPERTY(ReplicatedUsing=OnRep_IsBeingHooked, BlueprintReadOnly, Category = "Hook")
	bool bIsBeingHooked = false;

	/**
	 * @brief 현재 이 Luggage를 훅하고 있는 플레이어
	 * @details 중복 훅 방지 및 디버깅용
	 */
	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Hook")
	TObjectPtr<AActor> HookedBy = nullptr;

	/**
	 * @brief bIsBeingHooked 복제 시 호출되는 콜백
	 * @details 훅 상태 변화 시 비주얼 피드백 (Outline 등)
	 */
	UFUNCTION()
	void OnRep_IsBeingHooked();

protected:
	// Collision state replication (clients also disable physics when holder claims the luggage)
	UPROPERTY(ReplicatedUsing=OnRep_CollisionEnabled)
	bool bCollisionEnabled = true;

	UFUNCTION()
	void OnRep_CollisionEnabled();
	
	void ApplyCollisionState(bool bEnable);

	//--------------------------------------------------------------//
	// Luggage Info
	//--------------------------------------------------------------//

	// @brief 캐리어의 인덱스
	int32 SpawnIdx = -1;
	// @brief 캐리어의 색상
	FString Color = "";
	// @brief 캐리어의 무늬
	FString Pattern = "";
	
public:
	UFUNCTION(BlueprintCallable)
	void SetLuggageInfo(int32 InIdx, FString InColor, FString InPattern);

	FORCEINLINE int32 GetSpawnIdx() {return SpawnIdx;}
	FORCEINLINE FString GetColor() {return Color;}
	FORCEINLINE FString GetPattern() {return Pattern;}
	
public:
	// 색상의 인덱스값
	UPROPERTY(ReplicatedUsing=OnRep_ColorIdx)
	int32 ColorIdx = -1;
	// 무늬의 인덱스값
	UPROPERTY(ReplicatedUsing=OnRep_PatternIdx)
	int32 PatternIdx = -1;

	UFUNCTION()
	void OnRep_ColorIdx();

	UFUNCTION()
	void OnRep_PatternIdx();

	// 캐리어 색상 변경
	void ApplyColorToMesh(int32 InColorIdx);
	// 캐리어 무늬 변경
	void ApplyPatternToMesh(int32 InPatternIdx);

public:
	// Outline
	UFUNCTION(BlueprintCallable, Category = "Outline")
	void OutlineOn();
	UFUNCTION(BlueprintCallable, Category = "Outline")
	void OutlineOff();

	/// @brief 모든 컴포넌트의 충돌과 물리를 설정합니다.
	/// @param bEnable [in] true면 충돌 활성화, false면 비활성화
	/// @details Holder에서 Luggage의 상호작용을 제어하기 위해 사용됩니다.
	UFUNCTION(BlueprintCallable, Category = "Collision")
	void SetAllCollision(bool bEnable);

private:
	void UpdateWidget();
	bool bWidgetInitialized = false;
	
public:
	//--------------------------------------------------------------//
	// Read Quest Data
	//--------------------------------------------------------------//

	/// @brief 캐리어의 심볼 (문제1 정답)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest")
	FString Target1;

	/// @brief 캐리어의 색상 (문제2 정답)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest")
	FString Target2;
	
	/// @brief 플레이어가 캐리어를 선택했을 때 호출됩니다.
	/// @param Interactor [in] 상호작용을 시도한 액터 (플레이어)
	UFUNCTION(BlueprintCallable, Category = "Interaction")
	void OnInteract(AActor* Interactor);

	/// @brief 서버에 캐리어 선택을 알립니다.
	/// @param Player [in] 선택한 플레이어의 PlayerState
	UFUNCTION(Server, Reliable, WithValidation)
	void ServerNotifySelection(class APlayerState* Player);



public:
	void PlayTTSAudio();

private:
	bool bIsRequest;

	void RequestListenAudio(const FString& AudioText);
	void OnResponseListenAudio(FResponseListenAudio& ResponseData, bool bWasSuccessful);
};
