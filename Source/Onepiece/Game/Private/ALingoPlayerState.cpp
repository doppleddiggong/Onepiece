// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.

#include "ALingoPlayerState.h"

#include "APlayerControl.h"
#include "ASpeakStageActor.h"
#include "EngineUtils.h"
#include "UBroadcastManager.h"
#include "Net/UnrealNetwork.h"
#include "GameLogging.h"
#include "UMainWidget.h"
#include "Kismet/KismetSystemLibrary.h"

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
	DOREPLIFETIME(ALingoPlayerState, SpeakJudesResults);

	DOREPLIFETIME(ALingoPlayerState, bReadQuestCompleted);
	DOREPLIFETIME(ALingoPlayerState, bListenQuestCompleted);
	DOREPLIFETIME(ALingoPlayerState, bSpeakQuestCompleted);
	DOREPLIFETIME(ALingoPlayerState, bWriteQuestCompleted);
}

FString ALingoPlayerState::GetChatContext() const
{
	// TODO: GameMode, GameState, PlayerState 데이터를 참조하여 동적으로 생성
	// 예시:
	// - 현재 게임 상태 (메뉴, 인게임, 퀘스트 진행 등)
	// - 플레이어 진행도 (레벨, 완료한 퀘스트 등)
	// - 최근 대화 히스토리
	// - NPC 정보 등

	// 기본 Context 반환
	return TEXT("You are a helpful assistant.");
}

void ALingoPlayerState::RefreshQuestState()
{
	// 각 퀘스트 상태 결정: 완료[V], 진행중[=], 미시작[X]
	auto GetQuestStatus = [](bool bCompleted, bool bInProgress) -> const TCHAR*
	{
		if (bCompleted) return TEXT("V");
		if (bInProgress) return TEXT("=");
		return TEXT("X");
	};

	// R-L-S-W 퀘스트 상태 출력
	FString StatusMsg = FString::Printf(TEXT("%s %s %s %s"),
		GetQuestStatus(bReadQuestCompleted, bReadQuestIng),
		GetQuestStatus(bListenQuestCompleted, bListenQuestIng),
		GetQuestStatus(bSpeakQuestCompleted, bSpeakQuestIng),
		GetQuestStatus(bWriteQuestCompleted, bWriteQuestIng));

	// PRINT_STRING( TEXT("RLSW: %s"), *StatusMsg);
	
	if (APlayerControl* PC = Cast<APlayerControl>(GetOwner()))
	{
		PC->UpdateQuestOrderWidget(StatusMsg);
	}

	// Host(서버)의 위젯 업데이트 (OnRep는 클라이언트에서만 호출됨)
	if (HasAuthority())
	{
		if (APlayerControl* PC = Cast<APlayerControl>(GetOwner()))
		{
			PC->UpdateQuestInfoWidget();
		}
	}
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

void ALingoPlayerState::AddSpeakJudes(const FResponseSpeakingJudes& EvaluationResult)
{
	SpeakJudesResults.Add(EvaluationResult);

	PRINTLOG(TEXT("[PlayerState] Evaluation result added - Total results: %d, Feedback: %s"),
		SpeakJudesResults.Num(), *EvaluationResult.final_feedback);
}

void ALingoPlayerState::Server_NotifySpeakDataReady_Implementation()
{
	// 월드에서 SpeakStageActor를 찾습니다.
	if (UWorld* World = GetWorld())
	{
		for (TActorIterator<ASpeakStageActor> It(World); It; ++It)
		{
			ASpeakStageActor* SpeakStage = *It;
			if (SpeakStage)
			{
				// SpeakStage를 통해 퀘스트를 시작합니다.
				SpeakStage->StartStageForPlayer(this);
				PRINTLOG(TEXT("[ALingoPlayerState] Client is ready. Starting SpeakQuest for: %s"), *GetPlayerName());
				return; // 첫 번째로 찾은 SpeakStage를 사용하고 종료
			}
		}
	}

	PRINTLOG(TEXT("[ALingoPlayerState] Server_NotifySpeakDataReady - ASpeakStageActor not found in world!"));
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

void ALingoPlayerState::OnRep_SpeakScenarioData()
{
	OnUpdateSpeakScenarioData();
}

void ALingoPlayerState::OnUpdateSpeakScenarioData()
{
	Server_NotifySpeakDataReady();
}

bool ALingoPlayerState::GetCurrentSpeakQuestion(int32 StepIndex, FSpeakStageQuestion& Out) const
{
	if ( SpeakScenarioData.speak_quest_data.IsValidIndex(StepIndex))
	{
		Out = SpeakScenarioData.speak_quest_data[StepIndex];
		return true;
	}

	return false;
}

void ALingoPlayerState::SetSpeakQuestCompleted()
{
	if (!HasAuthority())
		return;

	bSpeakQuestCompleted = true;
	bSpeakQuestIng  = !bSpeakQuestCompleted;

	RefreshQuestState();
}

void ALingoPlayerState::SetReadQuestCompleted()
{
	if (!HasAuthority())
		return;

	bReadQuestCompleted = true;
	bReadQuestIng  = !bReadQuestCompleted;

	RefreshQuestState();
}

void ALingoPlayerState::SetListenQuestCompleted()
{
	if (!HasAuthority())
		return;

	bListenQuestCompleted = true;
	bListenQuestIng  = !bListenQuestCompleted;

	RefreshQuestState();
}

void ALingoPlayerState::SetWriteQuestCompleted()
{
	if (!HasAuthority())
		return;

	bWriteQuestCompleted = true;
	bWriteQuestIng  = !bWriteQuestCompleted;

	RefreshQuestState();
}

void ALingoPlayerState::SetReadQuestIng(bool bInProgress)
{
	if (!HasAuthority())
		return;

	bReadQuestIng = bInProgress;
	
	RefreshQuestState();
}

void ALingoPlayerState::SetListenQuestIng(bool bInProgress)
{
	if (!HasAuthority())
		return;

	bListenQuestIng = bInProgress;

	RefreshQuestState();
}

void ALingoPlayerState::SetSpeakQuestIng(bool bInProgress)
{
	if (!HasAuthority())
		return;

	bSpeakQuestIng = bInProgress;

	RefreshQuestState();
}

void ALingoPlayerState::SetWriteQuestIng(bool bInProgress)
{
	if (!HasAuthority())
		return;

	bWriteQuestIng = bInProgress;

	RefreshQuestState();
}

void ALingoPlayerState::OnRep_QuestState()
{
	// 클라이언트에서 퀘스트 상태가 변경되었을 때 PlayerController의 위젯 업데이트 함수 호출
	if (APlayerControl* PC = Cast<APlayerControl>(GetOwner()))
	{
		PC->UpdateQuestInfoWidget();

		// 각 퀘스트 상태 결정: 완료[V], 진행중[=], 미시작[X]
		auto GetQuestStatus = [](bool bCompleted, bool bInProgress) -> const TCHAR*
		{
			if (bCompleted) return TEXT("V");
			if (bInProgress) return TEXT("=");
			return TEXT("X");
		};

		// R-L-S-W 퀘스트 상태 출력
		FString StatusMsg = FString::Printf(TEXT("%s %s %s %s"),
			GetQuestStatus(bReadQuestCompleted, bReadQuestIng),
			GetQuestStatus(bListenQuestCompleted, bListenQuestIng),
			GetQuestStatus(bSpeakQuestCompleted, bSpeakQuestIng),
			GetQuestStatus(bWriteQuestCompleted, bWriteQuestIng));
	
		PC->UpdateQuestOrderWidget(StatusMsg);
	}
}