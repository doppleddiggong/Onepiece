// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.

#include "ALingoPlayerState.h"

#include "APlayerControl.h"
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
	DOREPLIFETIME(ALingoPlayerState, AttemptCount);

	DOREPLIFETIME(ALingoPlayerState, SelectedWord1);
	DOREPLIFETIME(ALingoPlayerState, SelectedWord2);
	DOREPLIFETIME(ALingoPlayerState, bWrongWord1);
	DOREPLIFETIME(ALingoPlayerState, bWrongWord2);

	// Speak Quest Data
	DOREPLIFETIME(ALingoPlayerState, SpeakScenarioData);
	DOREPLIFETIME(ALingoPlayerState, CurSpeakQuestStep);
	DOREPLIFETIME(ALingoPlayerState, SpeakJudesResults);
}


//--------------------------------------------------------------//
// Read Quest RPC Functions
//--------------------------------------------------------------//

void ALingoPlayerState::Server_SetSelectedWord1_Implementation(const FString& Word1)
{
	// Role 검증: OnlyQuestion2 역할은 심볼을 선택할 수 없음
	if (QuestRole == EQuestRole::OnlyQuestion2)
	{
		PRINTLOG(TEXT("[PlayerState] ServerSetSelectedSymbol - Access denied for OnlyQuestion2 role"));
		return;
	}

	SelectedWord1 = Word1;
	bWrongWord1 = false; // 새로 선택하면 오답 플래그 초기화

	PRINTLOG(TEXT("[PlayerState] Symbol selected: %s"), *Word1);
}

bool ALingoPlayerState::Server_SetSelectedWord1_Validate(const FString& Word1)
{
	// 빈 문자열도 허용 (선택 해제)
	return true;
}

void ALingoPlayerState::Server_SetSelectedWord2_Implementation(const FString& Word2)
{
	// Role 검증: OnlyQuestion1 역할은 색상을 선택할 수 없음
	if (QuestRole == EQuestRole::OnlyQuestion1)
	{
		PRINTLOG(TEXT("[PlayerState] ServerSetSelectedColor - Access denied for OnlyQuestion1 role"));
		return;
	}

	SelectedWord2 = Word2;
	bWrongWord2 = false; // 새로 선택하면 오답 플래그 초기화

	PRINTLOG(TEXT("[PlayerState] Color selected: %s"), *Word2);
}

bool ALingoPlayerState::Server_SetSelectedWord2_Validate(const FString& Word2)
{
	// 빈 문자열도 허용 (선택 해제)
	return true;
}

void ALingoPlayerState::Server_AddSpeakJudes_Implementation(const FResponseSpeakingJudes& EvaluationResult)
{
	SpeakJudesResults.Add(EvaluationResult);

	PRINTLOG(TEXT("[PlayerState] Evaluation result added - Total results: %d, Feedback: %s"),
		SpeakJudesResults.Num(), *EvaluationResult.final_feedback);
}

bool ALingoPlayerState::Server_AddSpeakJudes_Validate(const FResponseSpeakingJudes& EvaluationResult)
{
	return true;
}


//--------------------------------------------------------------//
// Read Quest OnRep Callbacks
//--------------------------------------------------------------//

void ALingoPlayerState::OnRep_QuestRole()
{
	PRINTLOG(TEXT("[PlayerState] OnRep_QuestRole: %s"), *ENUM_TO_NAME(EQuestRole, QuestRole));

	// PC한테 콜을 날리고,
	// PC가 판단해야해?
	// 플레이어마다 QuestRole 정해져있음

	APlayerControl* PC = Cast<APlayerControl>(GetOwner());
	if (PC)
	{
		PC->UpdateQuestRole(QuestRole);
	}
}

void ALingoPlayerState::OnRep_SelectedWord1()
{
	PRINTLOG(TEXT("[PlayerState] OnRep_SelectedSymbol: %s"), *SelectedWord1);
}

void ALingoPlayerState::OnRep_SelectedWord2()
{
	PRINTLOG(TEXT("[PlayerState] OnRep_SelectedColor: %s"), *SelectedWord2);
}

void ALingoPlayerState::OnRep_WrongWord1()
{
	PRINTLOG(TEXT("[PlayerState] OnRep_SymbolWrong: %s"), bWrongWord1 ? TEXT("true") : TEXT("false"));
}

void ALingoPlayerState::OnRep_WrongWord2()
{
	PRINTLOG(TEXT("[PlayerState] OnRep_ColorWrong: %s"), bWrongWord2 ? TEXT("true") : TEXT("false"));
}