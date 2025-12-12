// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AWheatly.generated.h"

class UBoxComponent;
class UStaticMeshComponent;

/// @brief Wheatly 애니메이션 타입
UENUM(BlueprintType)
enum class EWheatlyAnim : uint8
{
	PowerOn         UMETA(DisplayName = "Power On"),
	Talk            UMETA(DisplayName = "Talk"),
	Reaction_01     UMETA(DisplayName = "Reaction 01"),
	Reaction_02     UMETA(DisplayName = "Reaction 02"),
	Reaction_03     UMETA(DisplayName = "Reaction 03"),
	Reaction_04     UMETA(DisplayName = "Reaction 04"),
	Reaction_05     UMETA(DisplayName = "Reaction 05"),
	Max             UMETA(Hidden)
};

/// @brief Wheatly NPC 액터
/// @details Portal의 Wheatly 캐릭터를 구현한 NPC
/// - 턴 기반 대화 시스템 지원
/// - 애니메이션 재생 및 네트워크 동기화
/// - 상태에 따른 시각적 피드백 (눈 색상 변경)
UCLASS()
class ONEPIECE_API AWheatly : public AActor
{
	GENERATED_BODY()

	//----------------------------------------------------------//
	// Initialization
	//----------------------------------------------------------//

public:
	AWheatly();
	
	virtual void Tick(float DeltaSeconds) override;

protected:
	virtual void BeginPlay() override;

public:
	/// @brief 애니메이션 타입 설정
	/// @param InAnimType [in] 설정할 애니메이션 타입
	FORCEINLINE void SetAnimationType(EWheatlyAnim InAnimType) { AnimType = InAnimType; }

	/// @brief 특정 애니메이션 재생 (서버에서 호출)
	/// @param InAnimType [in] 재생할 애니메이션 타입
	UFUNCTION(BlueprintCallable, Category = "Animation")
	void PlayAnimation(EWheatlyAnim InAnimType);

protected:
	/// @brief 애니메이션 재생 (멀티캐스트 RPC)
	/// @param InAnimType [in] 재생할 애니메이션 타입
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_PlayAnimation(EWheatlyAnim InAnimType);

public:
	/// @brief SpeakStage 설정 (GameMode에서 호출)
	/// @param InSpeakStage [in] 연결할 SpeakStageActor
	UFUNCTION(BlueprintCallable, Category = "SpeakStage")
	void SetSpeakStage(class ASpeakStageActor* InSpeakStage);

	/// @brief SpeakQuest 시작 (서버에서만 호출)
	/// @param Player [in] 퀘스트를 시작할 플레이어
	void BeginSpeakQuest(class APlayerActor* Player);

	/// @brief SpeakQuest 완료 처리 (서버에서만 호출)
	/// @param Player [in] 퀘스트를 완료한 플레이어
	void CompleteSpeakQuest(class APlayerActor* Player);

protected:
	/// @brief 플레이어 상호작용 핸들러
	/// @param InteractingActor [in] 상호작용을 시도하는 액터
	UFUNCTION()
	void OnInteractionTriggered(class AActor* InteractingActor);

	/// @brief SpeakStage의 발화자 변경 이벤트 핸들러
	/// @param NewSpeaker [in] 새로운 발화자 (없으면 nullptr)
	UFUNCTION()
	void OnSpeakStageSpeakerChanged(class APlayerState* NewSpeaker);

private:
	void RequestSpeakScenario(class APlayerActor* Player);
	void OnResponseSpeakScenario(struct FResponseSpeakScenario& ResponseData, bool bWasSuccessful);
	
	/// @brief 눈 색상 변경
	/// @param newColor [in] 새로운 색상
	void ChangeEyeColor(FLinearColor newColor);

protected:
	/// @brief 스켈레탈 메시 컴포넌트
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<USkeletalMeshComponent> MeshComponent;

	/// @brief 플레이어 감지 영역
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UBoxComponent> PlayerDetectionZone;

	/// @brief 상호작용 중인 플레이어 표시기
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UStaticMeshComponent> InteractingPlayerIndicator;

	/// @brief 상호작용 컴포넌트
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<class UInteractableComponent> InteractableComp;

	/// @brief 위젯 컴포넌트 (상호작용 UI 표시)
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Components")
	TObjectPtr<class UWidgetComponent> WidgetComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<class UBoxComponent> BoxComp;
	
	/// @brief 베이스 머티리얼 (눈 발광)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Materials")
	TObjectPtr<UMaterialInterface> baseMaterial;

	/// @brief 동적 머티리얼 인스턴스 (런타임 색상 변경)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Transient, Category = "Materials", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UMaterialInstanceDynamic> dynamicMaterial;

	/// @brief 애니메이션 시퀀스 맵
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation")
	TMap<EWheatlyAnim, TObjectPtr<class UAnimSequence>> AnimSequences;

private:
	UPROPERTY()
	TObjectPtr<class ASpeakStageActor> SpeakStage;

	UPROPERTY()
	TObjectPtr<class APlayerActor> RequestPlayer;

	/// @brief 현재 애니메이션 타입
	EWheatlyAnim AnimType;
	
	/// @brief 현재 재생 중인 애니메이션 길이 (초)
	float CurAnimDuration;

	bool bShowDebugInfo = true;
};
