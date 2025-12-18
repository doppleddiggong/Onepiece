// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.

#pragma once

#include "CoreMinimal.h"
#include "NetworkData.h"
#include "GameFramework/Actor.h"
#include "ListenAnswer.generated.h"

UENUM(BlueprintType)
enum class EAnswerType : uint8
{
	None = 0,
	Food  = 1,
	City = 2
};

USTRUCT(BlueprintType)
struct FListenAnswerData
{
	GENERATED_BODY()
	/** 정답 타입 */
	EAnswerType AnswerType = EAnswerType::None;
	/** 시나리오 단어 정보 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FWordInfo word1;

	FListenAnswerData() {}

	FListenAnswerData(EAnswerType& InAnswerType, const FWordInfo& InWord1)
	: AnswerType(InAnswerType), word1(InWord1){}
};

UCLASS()
class ONEPIECE_API AListenAnswer : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AListenAnswer();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

public:
	// Components
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UStaticMeshComponent* Mesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UWidgetComponent* NameWidgetComp;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<class UInteractableComponent> InteractableComp;

public:
	//---------------------------------------------------
	// Data
	//---------------------------------------------------
	// 데이터 테이블
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
	class UDataTable* ListenDataTable;
	// 현재 입력된 데이터
	UPROPERTY(ReplicatedUsing = OnRep_AnswerData)
	FListenAnswerData AnswerData;

	UFUNCTION()
	void OnRep_AnswerData();
	
	void SetSpawnData(const FListenAnswerData& InData) { AnswerData = InData; }

	UFUNCTION()
	void OnOutlineStateChanged(bool bShouldShowOutline);

	void UpdateMesh();
	void UpdateNameWidget();
	
};
