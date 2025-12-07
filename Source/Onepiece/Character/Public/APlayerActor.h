// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.

/**
 * @file APlayerActor.h
 * @brief Declares the player-controlled character actor.
 */

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "IControllable.h"
#include "APlayerActor.generated.h"

/**
 * @brief Main character driven directly by the player.
 * @details Extends AGameCharacter with player-specific input and camera handling.
 * @ingroup Character
 */
UCLASS(Blueprintable, BlueprintType, ClassGroup=(Dopple))
class ONEPIECE_API APlayerActor : public ACharacter, public IControllable
{
	GENERATED_BODY()

public:
	APlayerActor();

protected:
	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void PossessedBy(AController* NewController) override;
	
	virtual void OnRep_Controller() override;

	UFUNCTION()
	void OnRep_LookPitch();

	UFUNCTION()
	void OnRep_AnotherValue();

	/// @brief Another 머티리얼 파라미터를 실제로 적용합니다 (서버/클라이언트 공통)
	void ApplyAnotherValue();

	UFUNCTION(BlueprintCallable, Category="Command")
	void RecoveryMovementMode(const EMovementMode InMovementMode);

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components|System")
	TObjectPtr<class UInteractionSystem> InteractionSystem;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components|System")
	TObjectPtr<class UHookSystem> HookSystem;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components|Hook")
	TObjectPtr<class UStaticMeshComponent> HookCable;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components|Hook")
	TObjectPtr<class UStaticMeshComponent> HookProjectileMesh;



	
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Camera", meta=(AllowPrivateAccess="true"))
	TObjectPtr<class USpringArmComponent> SpringArmComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Camera", meta=(AllowPrivateAccess="true"))
	TObjectPtr<class UCameraComponent> FollowCamera;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Camera", meta=(AllowPrivateAccess="true"))
	TObjectPtr<class UHookComponent> HookComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="RushAttack|Owner")
	TObjectPtr<class UCharacterMovementComponent> MoveComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Voice", meta=(AllowPrivateAccess="true"))
	TObjectPtr<class UVoiceConversationSystem> VoiceConversationSystem;

	
public: // Control Interface
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Command")
	void Cmd_Move(const FVector2D& Axis) override;
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Command")
	void Cmd_StopMove() override;
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Command")
	void Cmd_Look(const FVector2D& Axis) override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Command")
	void Cmd_Jump() override;
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Command")
	void Cmd_Run() override;

	/// @brief GPT 상호작용을 위한 음성 캡처를 시작합니다.
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Command")
	void Cmd_RecordStart() override;
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Command")
	void Cmd_RecordEnd() override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Command")
	void Cmd_Info() override;
	
	UFUNCTION(Server, Reliable)
	void ServerRPC_StopMove();
	UFUNCTION(NetMulticast, Reliable)
	void MulticastRPC_StopMove();
	UFUNCTION(Server, Reliable)
	void ServerRPC_DoJumpStart();
	UFUNCTION(NetMulticast, Reliable)
	void MulticastRPC_DoJumpStart();
	UFUNCTION(Server, Reliable)
	void ServerRPC_DoJump();
	UFUNCTION(NetMulticast, Reliable)
	void MulticastRPC_DoJump();
	UFUNCTION(Server, Reliable)
	void ServerRPC_DoRun();
	UFUNCTION(NetMulticast, Reliable)
	void MulticastRPC_DoRun();
	
public:
	FORCEINLINE bool GetIsRunning() { return bIsRunning; }
	FORCEINLINE bool GetIsJumpStart() { return bIsJumpStart; }
	
	/// @brief 게임 이벤트 메시지를 수신합니다.
	/// @param Message [in] 수신된 이벤트 메시지
	UFUNCTION(BlueprintCallable, Category="Event")
	void OnGameMessage(const FString& Message);

	/// @brief 클라이언트에서 게임 메시지를 표시합니다.
	/// @param Message [in] 표시할 메시지
	UFUNCTION(Client, Reliable)
	void ClientRPC_ShowGameMessage(const FString& Message);

	/// @brief TTS 오디오를 재생합니다. VoiceConversationSystem으로 전달합니다.
	/// @param AudioData [in] TTS로 생성된 오디오 데이터 (WAV)
	/// @return 재생 성공 여부
	UFUNCTION(BlueprintCallable, Category = "Voice")
	void PlayTTSAudio(const TArray<uint8>& AudioData);

	/// @brief 메인 위젯을 가져옵니다.
	/// @return 메인 위젯 인스턴스
	UFUNCTION(BlueprintPure, Category = "UI")
	class UMainWidget* GetMainWidget() const { return MainWidget; }

private:
	/// @brief 메인 위젯을 생성하고 뷰포트에 추가합니다.
	void CreateMainWidget();
	void CreateToastWidget();

	/// @brief 텔레포트 이벤트 핸들러
	/// @param TargetLocation 목표 위치
	UFUNCTION()
	void OnTeleportAllPlayers(FVector TargetLocation);

	UFUNCTION(Server, Reliable)
	void Server_Teleport(FVector TargetLocation);

	/// @brief 페이드 아웃 완료 후 텔레포트 실행
	UFUNCTION()
	void OnFadeOutCompleteForTeleport();

	/// @brief 텔레포트 목표 위치
	FVector PendingTeleportLocation;
	
public:
	// grab 시 들어올릴 위치
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Interaction")
	USceneComponent* HoldPosition;

	// 서버쪽 pitch 수동으로 동기화
	// bUsePawnControlRotation은 서버->클라로 전달 안됨
	UPROPERTY(ReplicatedUsing=OnRep_LookPitch)
	float LookPitch;

	/// @brief 2P 구분을 위한 머티리얼 파라미터 (0=1P, 1=2P)
	UPROPERTY(ReplicatedUsing=OnRep_AnotherValue)
	float AnotherValue;

private:
	/// @brief 메인 위젯 블루프린트 클래스
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<class UMainWidget> MainWidgetClass;

	/// @brief 메인 UI 위젯 인스턴스
	UPROPERTY()
	TObjectPtr<class UMainWidget> MainWidget;

	/// @brief 메인 위젯 블루프린트 클래스
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<class UToastWidget> ToastWidgetClass;

	/// @brief 메인 UI 위젯 인스턴스
	UPROPERTY()
	TObjectPtr<class UToastWidget> ToastWidget;

	
	// Movement 관련 변수
	float WalkSpeed = 200.f;
	float RunSpeed = 500.f;
	bool bIsRunning = false;
	bool bIsJumpStart = false;
};
