// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.

#pragma once

#include "CoreMinimal.h"
#include "ANPCBase.h"
#include "NPCExaminer.generated.h"

UCLASS()
class ONEPIECE_API ANPCExaminer : public ANPCBase
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ANPCExaminer();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
protected:
	// Material Instance
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<class UMaterialInterface> Material2;
	// Dynamic Material Instance
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<class UMaterialInstanceDynamic> DynamicMaterial2;
	
	// Eye Color Name
	FName EyeColorName = FName("EmissiveColor2");
	FLinearColor EyeColor = FLinearColor::Red;
	
	// Change Eye Color Red
	UFUNCTION(BlueprintCallable)
	void ChangeEyeColor();
	
	// Near Player Values
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<class AOwlPlayer> DetectedPlayer;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool bIsPlayerNear;
	
	// Collision
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<class USphereComponent> PlayerDetectSphereComp;
	
	// Overlap Player
	UFUNCTION()
	void OnSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	//----------------------------------------------------------//
	// Speak Stage System
	//----------------------------------------------------------//

protected:
	/// @brief 연결된 SpeakStage
	UPROPERTY()
	TObjectPtr<class ASpeakStageActor> SpeakStage;

public:
	/// @brief SpeakStage 설정 (GameMode에서 호출)
	/// @param InSpeakStage 연결할 SpeakStageActor
	UFUNCTION(BlueprintCallable, Category = "SpeakStage")
	void SetSpeakStage(class ASpeakStageActor* InSpeakStage);

	/// @brief 현재 질문 가져오기
	/// @return 현재 단계의 질문 문자열
	UFUNCTION(BlueprintCallable, Category = "SpeakStage")
	FString GetCurrentQuestion() const;
};
