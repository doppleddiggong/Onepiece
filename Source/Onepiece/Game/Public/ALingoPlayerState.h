// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "ALingoPlayerState.generated.h"

UCLASS()
class ONEPIECE_API ALingoPlayerState : public APlayerState
{
	GENERATED_BODY()

public:
	ALingoPlayerState();

protected:
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	
public:
	void SetToken(FString InToken);
	FString GetToken() { return AccessToken; }

	void SetUserName(FString InUserName);
	FString GetUserName() { return UserName; } 
	
private:
	FString AccessToken;
	FString UserName;
};
