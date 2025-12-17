// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.

#include "ASpeakStageActor.h"

#include "ALingoPlayerState.h"
#include "APlayerActor.h"
#include "APlayerControl.h"
#include "ULingoGameHelper.h"
#include "GameLogging.h"
#include "Net/UnrealNetwork.h"
#include "GameFramework/PlayerState.h"
#include "Engine/World.h"
#include "UDialogManager.h"
#include "UPopupManager.h"

ASpeakStageActor::ASpeakStageActor()
{
	// Replication 설정
	bReplicates = true;
	bAlwaysRelevant = true; // 모든 클라이언트에 항상 복제

	// Tick 비활성화 (이벤트 기반으로 동작)
	PrimaryActorTick.bCanEverTick = false;

	// 초기값 설정
	CurrentSpeaker = nullptr;
	CurrentStepIndex = 0;
}

void ASpeakStageActor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ASpeakStageActor, CurrentSpeaker);
	DOREPLIFETIME(ASpeakStageActor, CurrentStepIndex);
}

//----------------------------------------------------------
// RepNotify Functions
//----------------------------------------------------------

void ASpeakStageActor::OnRep_CurrentSpeaker()
{
	OnSpeakerChanged.Broadcast(CurrentSpeaker);
}

//----------------------------------------------------------
// Public Interface
//----------------------------------------------------------

void ASpeakStageActor::StartStageForPlayer(ALingoPlayerState* Player)
{
	if (!HasAuthority() || !Player)
		return;

	// 이미 사용 중인 경우
	if (CurrentSpeaker != nullptr)
		return;
	
	// 플레이어를 현재 발화자로 설정
	CurrentSpeaker = Player;
	CurrentStepIndex = 0;

	// 서버 측 리스너에게 즉시 알림
	OnSpeakerChanged.Broadcast(CurrentSpeaker);

	// 모든 클라이언트에게 SpeakQuest 시작 알림 브로드캐스트
	FString PlayerName = ULingoGameHelper::GetPlayerNameFromState(Player);
	Multicast_NotifySpeakQuestStarted(PlayerName);

	// 클라이언트에게 현재 단계 정보(Toast, TTS)를 전송하고 UI 업데이트를 요청합니다.
	if (APlayerControl* PC = Cast<APlayerControl>(CurrentSpeaker->GetOwner()))
	{
		// 클라이언트에게 StepIndex를 전달하여 스스로 UI와 TTS를 처리하도록 합니다.
		PC->Client_UpdateSpeakQuest(CurrentStepIndex);
		PRINTLOG(TEXT("[SpeakStage] Sent StepIndex %d to client for update."), CurrentStepIndex);
	}
}

void ASpeakStageActor::Multicast_NotifySpeakQuestStarted_Implementation(const FString& PlayerName)
{
	// 모든 클라이언트에서 Toast 메시지 표시
	if (UDialogManager* DM = UDialogManager::Get(GetWorld()))
	{
		FString Message = FString::Printf(TEXT("[%s] has started the inspection quest with the officer."), *PlayerName);
		DM->ShowToast(Message);
	}
}

void ASpeakStageActor::NotifyAnswerComplete(ALingoPlayerState* Player)
{
	if (!HasAuthority() || !Player)
		return;

	// 현재 발화자가 아니면 무시
	if (CurrentSpeaker != Player)
		return;

	// 다음 단계로 진행
	AdvanceStep();
}

int32 ASpeakStageActor::GetTotalQuestions() const
{
	if (ALingoPlayerState* PS = Cast<ALingoPlayerState>(CurrentSpeaker))
	{
		return PS->SpeakScenarioData.speak_quest_data.Num();
	}
	return 0;
}

FString ASpeakStageActor::GetCurrentQuestion() const
{
	if (!CurrentSpeaker)
		return TEXT("");

	// PlayerState에서 데이터 가져오기
	if (auto PS = Cast<ALingoPlayerState>(CurrentSpeaker))
	{
		if (PS->SpeakScenarioData.speak_quest_data.IsValidIndex(CurrentStepIndex))
			return PS->SpeakScenarioData.speak_quest_data[CurrentStepIndex].GetQuestionMessage();
	}

	return TEXT("");
}

int32 ASpeakStageActor::GetTotalQuestionsCount()
{
	if (ALingoPlayerState* PS = Cast<ALingoPlayerState>(CurrentSpeaker))
		return PS->SpeakScenarioData.speak_quest_data.Num();
	return 0;
}

void ASpeakStageActor::AdvanceStep()
{
	if (!HasAuthority())
	{
		return;	
	}

	CurrentStepIndex++;

	// 모든 질문 완료?
	auto TotalQuestionsCount = GetTotalQuestionsCount();
	if( CurrentStepIndex >= TotalQuestionsCount )
	{
		PRINTLOG(TEXT("[SpeakStage] All steps completed for: %s"), *ULingoGameHelper::GetPlayerNameFromState(CurrentSpeaker));
		// 스테이지 종료
		EndStage();
	}
	else
	{
		PRINTLOG(TEXT("[SpeakStage] Advanced to step: %d/%d"), CurrentStepIndex + 1, TotalQuestionsCount);

		// 클라이언트에게 현재 단계 정보(Toast, TTS)를 전송하고 UI 업데이트를 요청합니다.
		if (APlayerControl* PC = Cast<APlayerControl>(CurrentSpeaker->GetOwner()))
		{
			// 클라이언트에게 StepIndex를 전달하여 스스로 UI와 TTS를 처리하도록 합니다.
			PC->Client_UpdateSpeakQuest(CurrentStepIndex);
			PRINTLOG(TEXT("[SpeakStage] Sent StepIndex %d to client for update."), CurrentStepIndex);
		}
	}
}

void ASpeakStageActor::EndStage()
{
	if (!HasAuthority() || !CurrentSpeaker)
		return;

	// SpeakQuest 완료 처리 (PlayerState에 플래그 설정)
	if (ALingoPlayerState* PS = Cast<ALingoPlayerState>(CurrentSpeaker))
	{
		PS->SetSpeakQuestCompleted();
	}

	// 클라이언트 측에서 완료 UI 처리 및 위젯 업데이트를 하도록 단일 RPC 호출
	if (APlayerControl* PC = Cast<APlayerControl>(CurrentSpeaker->GetOwner()))
	{
		PC->Client_EndSpeakQuest();
		PRINTLOG(TEXT("[SpeakStage] Sent FinalizeSpeakQuest to client: %s"), *ULingoGameHelper::GetPlayerNameFromState(CurrentSpeaker));
	}
	
	// 서버 상태 초기화
	CurrentSpeaker = nullptr;
	CurrentStepIndex = 0;

	// 서버 측 리스너에게 알림
	OnSpeakerChanged.Broadcast(CurrentSpeaker);
}

bool ASpeakStageActor::IsMyTurn(ALingoPlayerState* PlayerState)
{
	if ( PlayerState == nullptr)
		return false;

	if ( CurrentSpeaker == nullptr)
		return false;
	
	return CurrentSpeaker == PlayerState;
}
