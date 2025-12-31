// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.

/**
 * @file APlayerActor.h
 * @brief Declares the player-controlled character actor.
 */

#pragma once

#include "CoreMinimal.h"
#include "CompassTargetInterface.h"
#include "GameFramework/Character.h"
#include "IControllable.h"
#include "NetworkData.h"
#include "EQuestRole.h"
#include "APlayerActor.generated.h"

/**
 * @brief Main character driven directly by the player.
 * @details Extends AGameCharacter with player-specific input and camera handling.
 * @ingroup Character
 */
UCLASS(Blueprintable, BlueprintType, ClassGroup=(Dopple))
class ONEPIECE_API APlayerActor : public ACharacter, public IControllable, public ICompassTargetInterface
{
	GENERATED_BODY()

public:
	APlayerActor();

protected:
	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void PossessedBy(AController* NewController) override;

	virtual void Tick(float DeltaTime) override;
	
	virtual void OnRep_Controller() override;

	UFUNCTION()
	void OnRep_LookPitch();

	UFUNCTION()
	void OnRep_AnotherValue();

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

	// grab 시 들어올릴 위치
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Interaction")
	TObjectPtr<class USceneComponent> HoldPosition;
	
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
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<class UChildActorComponent> miniOwlBot;

private:
	FVector basePos = FVector(238,135.5,60);
	FRotator baseRot = FRotator(-5.53,212.2,-0.2);
	
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
	class AMiniOwlBot* GetMiniOwlBot() const;
	
	void PlaySpeakInfo(int32 QuestStep);

	EQuestRole GetQuestRole();
	
	/// @brief 게임 이벤트 메시지를 수신합니다.
	/// @param Message [in] 수신된 이벤트 메시지
	UFUNCTION(BlueprintCallable, Category="Event")
	void OnGameMessage(const FString& Message);

	UFUNCTION(Server, Reliable)
	void ServerRPC_Teleport(FTransform TargetTransform);

	/// @brief SpeakJudge 결과를 서버로 전달하여 처리합니다 (Server RPC)
	/// @param Response [in] Client에서 받은 SpeakJudge 평가 결과
	UFUNCTION(Server, Reliable)
	void Server_NotifySpeakJudgeComplete(const struct FResponseSpeakingJudes& Response);

	/// @brief SpeakJudge 결과 팝업을 클라이언트에 표시합니다 (Client RPC)
	/// @param Response [in] 표시할 SpeakJudge 평가 결과
	UFUNCTION(Client, Reliable)
	void Client_ShowSpeakJudesPopup(const struct FResponseSpeakingJudes& Response);

	/// @brief SpeakJudge 팝업 확인 버튼 클릭을 서버에 알립니다 (Server RPC)
	/// @details 사용자가 결과를 확인한 후 다음 질문으로 진행하도록 SpeakStage에 알림
	UFUNCTION(Server, Reliable)
	void Server_NotifyConfirmSpeakJudes();

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

	void RequestListenAudio(const FString& AudioText);
	void RequestSpeakAudio(const FString& AudioText);

	/// @brief 플레이어 조작이 가능한지 확인합니다
	/// @return true면 조작 가능, false면 팝업으로 인해 조작 차단됨
	/// @details 팝업이 열려있고 해당 팝업이 플레이어 조작을 차단하는 경우 false 반환
	bool IsControlEnabled() const;

private:
	bool IsMainMap();

	/// @brief Another 머티리얼 파라미터를 실제로 적용합니다 (서버/클라이언트 공통)
	void ApplyAnotherValue();
	
	/// @brief 메인 위젯을 생성하고 뷰포트에 추가합니다.
	void CreateMainWidget();
	void CreateToastWidget();

	void OnResponseListenAudio(FResponseListenAudio& ResponseData, bool bWasSuccessful);
	void OnResponseSpeakAudio(FResponseListenAudio& ResponseData, bool bWasSuccessful);
	
	/// @brief 텔레포트 이벤트 핸들러
	/// @param TargetLocation 목표 위치
	UFUNCTION()
	void OnTeleportAllPlayers(FTransform TargetTransform);

	/// @brief 페이드 아웃 완료 후 텔레포트 실행
	UFUNCTION()
	void OnFadeOutCompleteForTeleport();

	// UFUNCTION()
	// void OnUpdateQuestInfo();
	//
	// UFUNCTION()
	// void OnUpdateQuestRole(EQuestRole QuestRole);

	UFUNCTION()
	void OnRoomIdUpdated(int64 NewRoomId);

	UFUNCTION()
	void OnRoomLevelUpdated(int32 NewRoomLevel);

	// UFUNCTION()
	// void OnListenResultUpdated( const FResponseListenResult& Result);
	//
	// UFUNCTION()
	// void OnReadResultUpdated(const FResponseReadResult& Result);

protected:
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

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<class UToastWidget> ToastWidgetClass;

	UPROPERTY()
	TObjectPtr<class UToastWidget> ToastWidget;

	/// @brief 텔레포트 목표 위치
	FTransform PendingTeleportTransform;
	
	// Movement 관련 변수
	float WalkSpeed = 200.f;
	float RunSpeed = 500.f;
	bool bIsRunning = false;
	bool bIsJumpStart = false;

	bool bIsRequest = false;

	// Compass
	UPROPERTY()
	TMap<AActor*, class UImage*> CompassMarkerMap;
	
	void UpdateCompassMarkers();
	FRotator FindRelativeRotationAtTarget(AActor* Target);
};
