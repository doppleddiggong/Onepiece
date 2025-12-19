// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.

#pragma once

#include "CoreMinimal.h"
#include "NetworkData.h"
#include "GameFramework/Actor.h"
#include "Food.generated.h"

/*
 * 이름은 Food지만, 빈 Food Capsule
 */

USTRUCT(BlueprintType)
struct FFoodCapsuleData
{
	GENERATED_BODY()

	/** 시나리오 단어 정보 */
	// 도시 이름
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FWordInfo word1;
	// 음식 이름
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FWordInfo word2;

	FFoodCapsuleData() {}

	FFoodCapsuleData(const FWordInfo& InWord1, const FWordInfo& InWord2)
		: word1(InWord1)
		, word2(InWord2)
	{}
};

UCLASS()
class ONEPIECE_API AFood : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AFood();

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

	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite)
	class UStaticMeshComponent* FoodMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UWidgetComponent* CityName;

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
	UPROPERTY(ReplicatedUsing = OnRep_CurrentFoodData)
	FFoodCapsuleData CurrentFoodData;

	UFUNCTION()
	void OnRep_CurrentFoodData();

	UFUNCTION()
	void OnOutlineStateChanged(bool bShouldShowOutline);

protected:
	/**
	 * @brief Widget에 음식 이름 업데이트
	 */
	void UpdateFoodWidget();

	/**
	 * @brief 음식 메시 업데이트 (DataTable에서 로드)
	 */
	void UpdateMesh();

public:
	void SetFoodMesh(FWordInfo InWord, UStaticMesh* InMesh);
	void SetCityName(FWordInfo InWord);
};
