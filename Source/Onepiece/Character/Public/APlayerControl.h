// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.

/**
 * @file APlayerControl.h
 * @brief APlayerControl 선언에 대한 Doxygen 주석을 제공합니다.
 */
#pragma once

#include "CoreMinimal.h"
#include "ALingoPlayerState.h"
#include "NetworkData.h"
#include "InputActionValue.h"
#include "GameFramework/PlayerController.h"
#include "APlayerControl.generated.h"

UCLASS(Blueprintable, BlueprintType, ClassGroup=(Dopple))
class ONEPIECE_API APlayerControl : public APlayerController
{
	GENERATED_BODY()

public:
	APlayerControl();

	void UpdateQuestRole(EQuestRole QuestRole);

	// --- User Info (레벨 전환에서 유지됨) ---

	bool HasUserInfo() const {  return GetUserId() > 0; }
	FString GetUserName() const { return UserInfo.username; }
	int32 GetUserId() const { return UserInfo.id; }
	const FResponseUserMe& GetUserInfo() const { return UserInfo; }

protected:
    virtual void BeginPlay() override;
    virtual void SetupInputComponent() override;

	// --- Input Assets ---
	UPROPERTY(EditDefaultsOnly, Category="Input")
	TObjectPtr<class UInputMappingContext> IMC_Default;	

	UPROPERTY(EditDefaultsOnly, Category="Input") TObjectPtr<class UInputAction> IA_Move;
	UPROPERTY(EditDefaultsOnly, Category="Input") TObjectPtr<class UInputAction> IA_Look;
	UPROPERTY(EditDefaultsOnly, Category="Input") TObjectPtr<class UInputAction> IA_Jump;
	UPROPERTY(EditDefaultsOnly, Category="Input") TObjectPtr<class UInputAction> IA_Record;
	UPROPERTY(EditDefaultsOnly, Category="Input") TObjectPtr<class UInputAction> IA_Grab;
	UPROPERTY(EditDefaultsOnly, Category="Input") TObjectPtr<class UInputAction> IA_Interact;
	UPROPERTY(EditDefaultsOnly, Category="Input") TObjectPtr<class UInputAction> IA_Run;
	UPROPERTY(EditDefaultsOnly, Category="Input") TObjectPtr<class UInputAction> IA_Info;
	UPROPERTY(EditDefaultsOnly, Category="Input") TObjectPtr<class UInputAction> IA_Hook;

	// --- Handlers ---
	void OnMove(const FInputActionValue& Value);
	void OnLook(const FInputActionValue& Value);
	
	void OnStopMove(const FInputActionValue& Value);
	void OnJump(const FInputActionValue& Value);
	void OnRun(const FInputActionValue& Value);

	void OnRecordPressed(const FInputActionValue& Value);
	void OnRecordReleased(const FInputActionValue& Value);

	void OnGrab(const FInputActionValue& Value);
	void OnGrabRelease(const FInputActionValue& Value);

	void OnInteract(const FInputActionValue& Value);

	void OnInfo(const FInputActionValue& Value);

	void OnHook(const FInputActionValue& Value);

	UFUNCTION(Server, Reliable)
	void Server_OnGrab();	

	UFUNCTION(Server, Reliable)
	void Server_OnGrabRelease();

	UFUNCTION(Server, Reliable)
	void Server_OnInteract();

	UFUNCTION(Server, Reliable)
	void Server_OnHook();

	UFUNCTION(Server, Reliable)
	void Server_SetUserInfo(const FResponseUserMe& InUserInfo);

	UFUNCTION(Server, Reliable)
	void Server_RequestDrop();


private:
	void RequestDrop(APlayerControl* Requester);

	UFUNCTION(Client, Reliable)
	void Client_ToastMessage(const FString& Message);
	
	void TEST_DropperDropProcess();
	void TEST_AddItemToBoxList();
	
private:
    class IControllable* GetControllable() const;
	
	/// @brief 사용자 정보 (레벨 전환에서도 유지됨)
	UPROPERTY()
	FResponseUserMe UserInfo;
};
