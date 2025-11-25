// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.

#pragma once

#include "CoreMinimal.h"
#include "UPopup_MsgBox.h"
#include "Macro.h"
#include "Subsystems/LocalPlayerSubsystem.h"
#include "UPopupManager.generated.h"


UENUM(Blueprintable)
enum class EPopupType : uint8
{
	Info, 

	Register,
	Login
};


UCLASS()
class ONEPIECE_API UPopupManager : public ULocalPlayerSubsystem
{
	GENERATED_BODY()

public:
	DEFINE_LOCALPLAYER_SUBSYSTEM_GETTER_INLINE(UPopupManager);
	
	UPopupManager();

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "UI")
	TSubclassOf<UUserWidget> PopupClass;

	UPROPERTY()
	class UPopup_MsgBox* Popup_MsgBox;
	
	UPROPERTY()
	class UPopup_InputMsg* Popup_InputMsg;

public:
	void ShowMessageBox( const FString& InTitle, const FString& InDescription,
		EMsgBoxType InType, const FOnMsgBoxOkDelegate& InOkDelegate);

	void ShowMessageBox(
		const FString& InTitle, const FString& InDescription,
		EMsgBoxType InType, const FOnMsgBoxOkDelegate& InOkDelegate, const FOnMsgBoxCancelDelegate& InCancelDelegate );
	
	UFUNCTION(BlueprintCallable)
	void OpenPopup(const EPopupType InType);
	
	// 메세지 박스 숨기기
	UFUNCTION(BlueprintCallable)
	void HideMessageBox();
	
protected:
	// 월드에 메세지 박스 없을 경우 생성
	void EnsureWidgetForWorld(UWorld* World);
};