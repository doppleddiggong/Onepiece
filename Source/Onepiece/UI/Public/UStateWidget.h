// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UStateWidget.generated.h"

UCLASS(BlueprintType, Blueprintable)
class ONEPIECE_API UStateWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    void InitWidget();

    void UpdateUserName(int32 InUserId, const FString& InUserName);
    
protected:
    UPROPERTY(meta = (BindWidget), BlueprintReadOnly, Category = "State|NickName")
    TObjectPtr<class UBorder> Border_State;

    UPROPERTY(meta = (BindWidget), BlueprintReadOnly, Category = "State|NickName")
    TObjectPtr<class UTextBlock> Txt_UserId;

    UPROPERTY(meta = (BindWidget), BlueprintReadOnly, Category = "State|NickName")
    TObjectPtr<class UTextBlock> Txt_UserName;
};
