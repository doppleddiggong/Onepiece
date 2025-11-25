// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.

#include "ALingoPlayerState.h"
#include "UBroadcastManager.h"
#include "Net/UnrealNetwork.h"
#include "GameLogging.h"

ALingoPlayerState::ALingoPlayerState()
{
}

void ALingoPlayerState::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// Read Quest Data
	DOREPLIFETIME(ALingoPlayerState, QuestRole);
	DOREPLIFETIME(ALingoPlayerState, SelectedSymbol);
	DOREPLIFETIME(ALingoPlayerState, SelectedColor);
	DOREPLIFETIME(ALingoPlayerState, bSymbolWrong);
	DOREPLIFETIME(ALingoPlayerState, bColorWrong);
	DOREPLIFETIME(ALingoPlayerState, AttemptCount);
}

void ALingoPlayerState::SetToken(FString InToken)
{
	this->AccessToken = InToken;
}

void ALingoPlayerState::SetUserName(FString InUserName)
{
	this->UserName = InUserName;

	// BroadcastManager를 통해 모든 구독자에게 알림
	if (UBroadcastManager* BroadcastManager = UBroadcastManager::Get(GetWorld()))
	{
		BroadcastManager->SendUserNameChanged(UserName);
	}
}

//--------------------------------------------------------------//
// Read Quest RPC Functions
//--------------------------------------------------------------//

void ALingoPlayerState::ServerSetSelectedSymbol_Implementation(const FString& Symbol)
{
	// Role 검증: OnlyQuestion2 역할은 심볼을 선택할 수 없음
	if (QuestRole == EReadQuestRole::OnlyQuestion2)
	{
		PRINTLOG(TEXT("[PlayerState] ServerSetSelectedSymbol - Access denied for OnlyQuestion2 role"));
		return;
	}

	SelectedSymbol = Symbol;
	bSymbolWrong = false; // 새로 선택하면 오답 플래그 초기화

	PRINTLOG(TEXT("[PlayerState] Symbol selected: %s"), *Symbol);
}

bool ALingoPlayerState::ServerSetSelectedSymbol_Validate(const FString& Symbol)
{
	// 빈 문자열도 허용 (선택 해제)
	return true;
}

void ALingoPlayerState::ServerSetSelectedColor_Implementation(const FString& Color)
{
	// Role 검증: OnlyQuestion1 역할은 색상을 선택할 수 없음
	if (QuestRole == EReadQuestRole::OnlyQuestion1)
	{
		PRINTLOG(TEXT("[PlayerState] ServerSetSelectedColor - Access denied for OnlyQuestion1 role"));
		return;
	}

	SelectedColor = Color;
	bColorWrong = false; // 새로 선택하면 오답 플래그 초기화

	PRINTLOG(TEXT("[PlayerState] Color selected: %s"), *Color);
}

bool ALingoPlayerState::ServerSetSelectedColor_Validate(const FString& Color)
{
	// 빈 문자열도 허용 (선택 해제)
	return true;
}

//--------------------------------------------------------------//
// Read Quest OnRep Callbacks
//--------------------------------------------------------------//

void ALingoPlayerState::OnRep_SelectedSymbol()
{
	PRINTLOG(TEXT("[PlayerState] OnRep_SelectedSymbol: %s"), *SelectedSymbol);

	// BroadcastManager를 통해 UI 업데이트 이벤트 브로드캐스트
	if (UBroadcastManager* BroadcastManager = UBroadcastManager::Get(GetWorld()))
	{
		// 추후 BroadcastManager에 퀘스트 선택 상태 변경 이벤트 추가 필요
	}
}

void ALingoPlayerState::OnRep_SelectedColor()
{
	PRINTLOG(TEXT("[PlayerState] OnRep_SelectedColor: %s"), *SelectedColor);

	// BroadcastManager를 통해 UI 업데이트 이벤트 브로드캐스트
	if (UBroadcastManager* BroadcastManager = UBroadcastManager::Get(GetWorld()))
	{
		// 추후 BroadcastManager에 퀘스트 선택 상태 변경 이벤트 추가 필요
	}
}

void ALingoPlayerState::OnRep_SymbolWrong()
{
	PRINTLOG(TEXT("[PlayerState] OnRep_SymbolWrong: %s"), bSymbolWrong ? TEXT("true") : TEXT("false"));

	// BroadcastManager를 통해 UI 업데이트 이벤트 브로드캐스트
	if (UBroadcastManager* BroadcastManager = UBroadcastManager::Get(GetWorld()))
	{
		// 추후 BroadcastManager에 오답 플래그 변경 이벤트 추가 필요
	}
}

void ALingoPlayerState::OnRep_ColorWrong()
{
	PRINTLOG(TEXT("[PlayerState] OnRep_ColorWrong: %s"), bColorWrong ? TEXT("true") : TEXT("false"));

	// BroadcastManager를 통해 UI 업데이트 이벤트 브로드캐스트
	if (UBroadcastManager* BroadcastManager = UBroadcastManager::Get(GetWorld()))
	{
		// 추후 BroadcastManager에 오답 플래그 변경 이벤트 추가 필요
	}
}
