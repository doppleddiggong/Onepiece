// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "APopupTesterActor.generated.h"

UCLASS()
class ONEPIECE_API APopupTesterActor : public AActor
{
	GENERATED_BODY()

public:
	APopupTesterActor();
	
	UFUNCTION(BlueprintCallable, CallInEditor, Category = "TEST|Message")
	void ToastMsg();
	UFUNCTION(BlueprintCallable, CallInEditor, Category = "TEST|Message")
	void OKCancelMsgBox();
	UFUNCTION(BlueprintCallable, CallInEditor, Category = "TEST|Message")
	void OKMsgBox();

	UFUNCTION(BlueprintCallable, CallInEditor, Category = "TEST|Message")
	void PopupInputMsg();
	UFUNCTION(BlueprintCallable, CallInEditor, Category = "TEST|Message")
	void ReadQuest();
    
	void OnOK();
	void OnCancel();
};
