// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.

#pragma once

#include "CoreMinimal.h"
#include "Macro.h"
#include "NetworkData.h"
#include "Subsystems/LocalPlayerSubsystem.h"
#include "MessageBoxManager.generated.h"

/**
 * 
 */
UENUM(Blueprintable)
enum class EMessageBoxType : uint8
{
	Info, 
	Register,
	LogIn
};


UCLASS()
class ONEPIECE_API UMessageBoxManager : public ULocalPlayerSubsystem
{
	GENERATED_BODY()

public:
	DEFINE_LOCALPLAYER_SUBSYSTEM_GETTER_INLINE(UMessageBoxManager);
	
	UMessageBoxManager();

public:
	// UI
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "UI")
	TSubclassOf<UUserWidget> MessageBoxClass;

	UPROPERTY()
	class UMessageBox* MessageBox;

public:
	// 타입에 따른 메세지 박스 보여주기
	UFUNCTION(BlueprintCallable)
	void ShowMessageBox(EMessageBoxType Type,
		const FString& Title, const FString& Description);
	
	// 메세지 박스 숨기기
	UFUNCTION(BlueprintCallable)
	void HideMessageBox();
	
protected:
	// 월드에 메세지 박스 없을 경우 생성
	void EnsureWidgetForWorld(UWorld* World);
	
	// 이름 네트워크와 연결 헬퍼 함수
	// Register 타입 Input 처리
	UFUNCTION()
	void RegisterUserName(const FString& UserInput);
	// LogIn 타입 Input 처리
	UFUNCTION()
	void GetUserToken(const FString& UserInput);
	
	UFUNCTION()
	void OnResponseUserRegister(FResponseUserRegister& ResponseData, bool bWasSuccessful);
	UFUNCTION()
	void OnResponseUserToken(FResponseUserToken& ResponseData, bool bWasSuccessful);
};
