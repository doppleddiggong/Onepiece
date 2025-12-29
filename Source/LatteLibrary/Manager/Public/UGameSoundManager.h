// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.

/**
 * @file UGameSoundManager.h
 * @brief UGameSoundManager 클래스를 선언합니다.
 */
#pragma once

#include "CoreMinimal.h"
#include "Macro.h"
#include "EGameSoundType.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "UGameSoundManager.generated.h"

/**
 * @brief 게임 내 모든 사운드 재생을 관리하는 중앙 사운드 관리 서브시스템입니다.
 * @details 2D 및 3D 사운드 재생, 중지 기능을 제공하며, 특히 대화 음성과 같이 단일 인스턴스만 허용되어야 하는 사운드를 별도로 제어합니다.
 */
UCLASS()
class LATTELIBRARY_API UGameSoundManager : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    DEFINE_SUBSYSTEM_GETTER_INLINE(UGameSoundManager);

    UGameSoundManager();
	
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	UFUNCTION(BlueprintCallable, Category="Sound")
    void PlaySound(EGameSoundType Type, FVector Location);

	UFUNCTION(BlueprintCallable, Category="Sound")
    void PlaySound2D(EGameSoundType Type);

	UFUNCTION(BlueprintCallable, Category="Sound")
    void StopSound2D(const EGameSoundType Type);

	// 대화 음성 재생 (기존 대화 음성을 중지하고 새로운 음성 재생)
	UFUNCTION(BlueprintCallable, Category="Sound")
	UAudioComponent* PlayConversationVoice(USoundBase* Sound);

	// 현재 재생 중인 대화 음성 중지
	UFUNCTION(BlueprintCallable, Category="Sound")
	void StopConversationVoice();

	// 대화 음성이 재생 중인지 확인
	UFUNCTION(BlueprintCallable, Category="Sound")
	bool IsConversationVoicePlaying() const;

	/**
	 * @brief BGM을 재생합니다. 이미 같은 BGM이 재생 중이면 유지하고, 다른 BGM이면 전환합니다.
	 * @param Type 재생할 BGM 타입
	 */
	UFUNCTION(BlueprintCallable, Category="Sound")
	void PlayBGM(EGameSoundType Type);

	/**
	 * @brief 현재 재생 중인 BGM을 중지합니다.
	 */
	UFUNCTION(BlueprintCallable, Category="Sound")
	void StopBGM();

private:
	UPROPERTY(EditDefaultsOnly, Category = "Sound", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class USoundData> SoundAsset;

	UPROPERTY(EditAnywhere, Category = "Sound")
	TMap<EGameSoundType, TObjectPtr<class USoundBase>> SoundData;

	UPROPERTY()
	TMap<EGameSoundType, UAudioComponent*> ActiveSounds;

	// 현재 재생 중인 대화 음성 (MegaPopup, DasanExplain, PlayerAsk 공유)
	UPROPERTY()
	TObjectPtr<UAudioComponent> ConversationVoice;

	// 현재 재생 중인 BGM
	UPROPERTY()
	TObjectPtr<UAudioComponent> CurrentBGM;

	// 현재 재생 중인 BGM 타입
	EGameSoundType CurrentBGMType;
};
