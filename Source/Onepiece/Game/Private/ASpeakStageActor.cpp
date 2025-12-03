// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.

#include "ASpeakStageActor.h"

#include "EGameSoundType.h"
#include "GameLogging.h"
#include "Net/UnrealNetwork.h"
#include "GameFramework/PlayerState.h"
#include "Engine/World.h"
#include "UDialogManager.h"
#include "UGameSoundManager.h"

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
	TotalQuestions = 0;
}

void ASpeakStageActor::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority())
	{
		// 서버에서만 테스트 데이터 생성
		CreateTestScenarioData();
		PRINTLOG(TEXT("[SpeakStage] BeginPlay - Test scenario data created"));
	}
}

void ASpeakStageActor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ASpeakStageActor, CurrentSpeaker);
	DOREPLIFETIME(ASpeakStageActor, CurrentStepIndex);
	DOREPLIFETIME(ASpeakStageActor, PlayerQueue);
}

//----------------------------------------------------------
// RepNotify Functions
//----------------------------------------------------------

void ASpeakStageActor::OnRep_CurrentSpeaker()
{
	if (CurrentSpeaker)
	{
		PRINTLOG(TEXT("[SpeakStage] Current Speaker Changed: %s"), *CurrentSpeaker->GetPlayerName());
		// UI 업데이트는 외부에서 처리 (UMainWidget::UpdateSpeakWidget)
	}
	else
	{
		PRINTLOG(TEXT("[SpeakStage] No Current Speaker (All players completed or waiting)"));
	}
}

void ASpeakStageActor::OnRep_CurrentStepIndex()
{
	PRINTLOG(TEXT("[SpeakStage] OnRep_CurrentStepIndex - Step Changed: %d/%d"), CurrentStepIndex + 1, TotalQuestions);

	// 클라이언트에서 Toast 표시
	ShowCurrentQuestionToast();
}

//----------------------------------------------------------
// Public Interface
//----------------------------------------------------------

void ASpeakStageActor::PlayStart(const TArray<APlayerState*>& Players)
{
	if (!HasAuthority())
	{
		PRINTLOG(TEXT("[SpeakStage] PlayStart - Not called on server!"));
		return;
	}

	// 플레이어 큐 초기화
	PlayerQueue.Empty();

	for (APlayerState* Player : Players)
	{
		if (Player)
		{
			PlayerQueue.Add(Player);
			PRINTLOG(TEXT("[SpeakStage] PlayStart - Player added to queue: %s"), *Player->GetPlayerName());
		}
	}

	// 첫 번째 플레이어를 CurrentSpeaker로 설정
	if (PlayerQueue.Num() > 0)
	{
		CurrentSpeaker = PlayerQueue[0];
		CurrentStepIndex = 0;

		PRINTLOG(TEXT("[SpeakStage] PlayStart - First speaker assigned: %s (Total players: %d)"),
		         *CurrentSpeaker->GetPlayerName(), PlayerQueue.Num());

		// 서버에서도 첫 질문 Toast 표시 (OnRep는 클라이언트만 호출되므로)
		ShowCurrentQuestionToast();
	}
	else
	{
		CurrentSpeaker = nullptr;
		PRINTLOG(TEXT("[SpeakStage] PlayStart - No players in queue!"));
	}
}

void ASpeakStageActor::Server_RequestSpeak_Implementation(APlayerState* Player)
{
	if (!HasAuthority() || !Player)
	{
		return;
	}

	// 현재 발화자가 아니면 거부
	if (CurrentSpeaker != Player)
	{
		PRINTLOG(TEXT("[SpeakStage] RequestSpeak - Not your turn: %s (Current: %s)"),
		         *Player->GetPlayerName(),
		         CurrentSpeaker ? *CurrentSpeaker->GetPlayerName() : TEXT("None"));
		return;
	}

	// 발화 권한 승인
	PRINTLOG(TEXT("[SpeakStage] Speak permission granted: %s (Step: %d/%d)"),
	         *Player->GetPlayerName(), CurrentStepIndex + 1, TotalQuestions);

	// 여기서 추가 처리 가능 (STT 시작 등)
	// 필요시 ClientRPC로 승인 알림 전송 가능
}

bool ASpeakStageActor::Server_RequestSpeak_Validate(APlayerState* Player)
{
	return Player != nullptr;
}

void ASpeakStageActor::Server_NotifyAnswerComplete_Implementation(APlayerState* Player)
{
	if (!HasAuthority() || !Player)
	{
		return;
	}

	// 현재 발화자가 아니면 무시
	if (CurrentSpeaker != Player)
	{
		PRINTLOG(TEXT("[SpeakStage] NotifyAnswerComplete - Invalid speaker: %s"), *Player->GetPlayerName());
		return;
	}

	PRINTLOG(TEXT("[SpeakStage] Answer complete: %s (Step: %d/%d)"),
	         *Player->GetPlayerName(), CurrentStepIndex + 1, TotalQuestions);

	// 다음 단계로 진행
	AdvanceStep();
}

FString ASpeakStageActor::GetCurrentQuestion() const
{
	if (Questions.IsValidIndex(CurrentStepIndex))
	{
		return Questions[CurrentStepIndex];
	}

	return TEXT("");
}

//----------------------------------------------------------
// Internal Logic
//----------------------------------------------------------

void ASpeakStageActor::AdvanceStep()
{
	if (!HasAuthority())
	{
		return;
	}

	CurrentStepIndex++;

	// 모든 질문 완료?
	if (CurrentStepIndex >= TotalQuestions)
	{
		PRINTLOG(TEXT("[SpeakStage] All steps completed for: %s"), *CurrentSpeaker->GetPlayerName());

		// 다음 플레이어로 전환
		AdvanceToNextPlayer();
	}
	else
	{
		PRINTLOG(TEXT("[SpeakStage] Advanced to step: %d/%d"), CurrentStepIndex + 1, TotalQuestions);

		// 서버에서도 Toast 표시 (OnRep는 클라이언트만 호출됨)
		ShowCurrentQuestionToast();

		// RepNotify로 모든 클라이언트에 자동 전파
	}
}

void ASpeakStageActor::AdvanceToNextPlayer()
{
	if (!HasAuthority())
	{
		return;
	}

	if (!CurrentSpeaker)
	{
		return;
	}

	// 현재 플레이어를 큐에서 제거
	PlayerQueue.Remove(CurrentSpeaker);
	PRINTLOG(TEXT("[SpeakStage] Player completed: %s (Remaining: %d)"),
	         *CurrentSpeaker->GetPlayerName(), PlayerQueue.Num());

	// 다음 플레이어 설정
	if (PlayerQueue.Num() > 0)
	{
		CurrentSpeaker = PlayerQueue[0];
		CurrentStepIndex = 0; // 처음부터 시작

		PRINTLOG(TEXT("[SpeakStage] Next speaker: %s"), *CurrentSpeaker->GetPlayerName());

		// 다음 플레이어의 첫 질문 Toast 표시
		ShowCurrentQuestionToast();
	}
	else
	{
		CurrentSpeaker = nullptr;
		CurrentStepIndex = 0;

		PRINTLOG(TEXT("[SpeakStage] All players completed! Stage finished."));

		// 모든 단계 완료 Toast 표시
		if (UDialogManager* DM = UDialogManager::Get(GetWorld()))
		{
			DM->ShowToast(TEXT("🎉 입국 심사가 완료되었습니다!"));
			PRINTLOG(TEXT("[SpeakStage] Completion toast displayed"));
		}

		// 여기서 완료 이벤트 브로드캐스트 가능
		// 예: OnAllPlayersCompleted.Broadcast();
	}
}

void ASpeakStageActor::ShowCurrentQuestionToast()
{
	// 현재 질문 가져오기
	FString CurrentQuestion = GetCurrentQuestion();
	if (CurrentQuestion.IsEmpty())
	{
		return;
	}

	// Toast 메시지 생성
	FString ToastMessage = FString::Printf(TEXT("[%d/%d] %s"),
		CurrentStepIndex + 1,
		TotalQuestions,
		*CurrentQuestion);

	// DialogManager를 통해 Toast 표시
	if (UDialogManager* DM = UDialogManager::Get(GetWorld()))
	{
		DM->ShowToast(ToastMessage);

		UGameSoundManager::Get(GetWorld())->PlaySound2D( Questions_Voice[CurrentStepIndex] );
		
		PRINTLOG(TEXT("[SpeakStage] Toast displayed: %s"), *ToastMessage);
	}
	else
	{
		PRINTLOG(TEXT("[SpeakStage] DialogManager not found, cannot show toast"));
	}
}

void ASpeakStageActor::CreateTestScenarioData()
{
	if (!HasAuthority())
	{
		return;
	}

	// 테스트용 입국 심사 질문 생성
	Questions.Empty();
	Questions.Add(TEXT("What is your name?"));
	Questions.Add(TEXT("Where are you from?"));
	Questions.Add(TEXT("What is the purpose of your visit?"));
	Questions.Add(TEXT("How long will you stay?"));
	Questions.Add(TEXT("Where will you be staying?"));

	Questions_Voice.Empty();
	Questions_Voice.Add(EGameSoundType::What_is_your_name);
	Questions_Voice.Add(EGameSoundType::Where_are_you_from);
	Questions_Voice.Add(EGameSoundType::What_is_the_purpose_of_your_visit);
	Questions_Voice.Add(EGameSoundType::How_long_will_you_stay);
	Questions_Voice.Add(EGameSoundType::Where_will_you_be_staying);

	
	TotalQuestions = Questions.Num();

	PRINTLOG(TEXT("[SpeakStage] Test scenario data created: %d questions"), TotalQuestions);
}
