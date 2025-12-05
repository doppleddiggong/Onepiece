// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "USpeakStageSubsystem.generated.h"

/**
 * @brief Speak Stage 시스템 관리 Subsystem
 * @details SpeakStageActor와 NPC Examiner 생성 및 관리
 *
 * 사용 방법:
 * - GameMode나 다른 곳에서 GetWorld()->GetSubsystem<USpeakStageSubsystem>()로 접근
 * - Initialize()에서 자동으로 SpeakStage와 NPC 생성
 * - BeginSpeakQuest()로 퀘스트 시작
 */
UCLASS(Config = Game)
class ONEPIECE_API USpeakStageSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	/// @brief 생성자
	USpeakStageSubsystem();

	/// @brief Subsystem 초기화 (World가 준비된 후 자동 호출)
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	virtual void OnWorldBeginPlay(UWorld& InWorld) override;  // 추가!                                                                                                                                                                                            
	
	/// @brief Subsystem 종료 (World 정리 시 자동 호출)
	virtual void Deinitialize() override;

protected:
	//----------------------------------------------------------
	// Runtime Instances
	//----------------------------------------------------------

	/// @brief 생성된 SpeakStage 인스턴스
	UPROPERTY()
	TObjectPtr<class ASpeakStageActor> SpeakStageActor;

	/// @brief 생성된 Examiner 인스턴스
	UPROPERTY()
	TObjectPtr<class ANPCExaminer> Examiner;

	/// @brief SpeakStage 시스템이 초기화되었는지 여부
	UPROPERTY()
	bool bIsInitialized;

public:
	//----------------------------------------------------------
	// Public Interface
	//----------------------------------------------------------

	/**
	 * @brief SpeakStage 시스템 생성 (SpeakStage + NPC)
	 * @details Initialize()에서 자동 호출되지만, 수동으로 재생성할 때도 사용 가능
	 */
	UFUNCTION(BlueprintCallable, Category = "SpeakStage")
	void CreateSpeakStageSystem();

	/**
	 * @brief Speak 퀘스트 시작
	 * @param InStageIndex 스테이지 인덱스
	 *
	 * 모든 플레이어를 수집하여 SpeakStage->PlayStart() 호출
	 */
	UFUNCTION(BlueprintCallable, Category = "SpeakStage")
	void BeginSpeakQuest(int32 InStageIndex);

	/**
	 * @brief SpeakStage 참조 가져오기
	 * @return SpeakStageActor 인스턴스 (없으면 nullptr)
	 */
	UFUNCTION(BlueprintCallable, Category = "SpeakStage")
	class ASpeakStageActor* GetSpeakStage() const { return SpeakStageActor; }

	/**
	 * @brief Examiner NPC 참조 가져오기
	 * @return NPCExaminer 인스턴스 (없으면 nullptr)
	 */
	UFUNCTION(BlueprintCallable, Category = "SpeakStage")
	class ANPCExaminer* GetExaminer() const { return Examiner; }

	/**
	 * @brief 시스템 초기화 여부 확인
	 * @return 초기화되었으면 true
	 */
	UFUNCTION(BlueprintCallable, Category = "SpeakStage")
	bool IsInitialized() const { return bIsInitialized; }

private:
	void InitActor();
};
