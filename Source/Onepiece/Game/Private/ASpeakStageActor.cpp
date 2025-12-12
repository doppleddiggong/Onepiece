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
	currentSpeaker = nullptr;
	currentStepIndex = 0;
	totalQuestions = 0;
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

	DOREPLIFETIME(ASpeakStageActor, currentSpeaker);
	DOREPLIFETIME(ASpeakStageActor, currentStepIndex);
}

//----------------------------------------------------------
// RepNotify Functions
//----------------------------------------------------------

void ASpeakStageActor::OnRep_CurrentSpeaker()
{
	if (currentSpeaker)
	{
		PRINTLOG(TEXT("[SpeakStage] Current Speaker Changed: %s"), *currentSpeaker->GetPlayerName());
		// UI 업데이트는 외부에서 처리 (UMainWidget::UpdateSpeakWidget)
	}
	else
	{
		PRINTLOG(TEXT("[SpeakStage] Stage is now available."));
	}
	OnSpeakerChanged.Broadcast(currentSpeaker);
}

void ASpeakStageActor::OnRep_CurrentStepIndex()
{
	PRINTLOG(TEXT("[SpeakStage] OnRep_CurrentStepIndex - Step Changed: %d/%d"), currentStepIndex + 1, totalQuestions);

	// 클라이언트에서 Toast 표시
	ShowCurrentQuestionToast();
}

//----------------------------------------------------------
// Public Interface
//----------------------------------------------------------

void ASpeakStageActor::StartStageForPlayer(APlayerState* Player)
{
	if (!HasAuthority())
	{
		PRINTLOG(TEXT("[SpeakStage] StartStageForPlayer - Not called on server!"));
		return;
	}

	if (!Player)
	{
		PRINTLOG(TEXT("[SpeakStage] StartStageForPlayer - Player is null!"));
		return;
	}

	// 이미 사용 중인 경우
	if (currentSpeaker != nullptr)
	{
		PRINTLOG(TEXT("[SpeakStage] StartStageForPlayer - Stage is already busy with player: %s"), *currentSpeaker->GetPlayerName());
		return;
	}
	
	// 플레이어를 현재 발화자로 설정
	currentSpeaker = Player;
	currentStepIndex = 0;

	// 서버 측 리스너에게 즉시 알림
	OnSpeakerChanged.Broadcast(currentSpeaker);

	PRINTLOG(TEXT("[SpeakStage] StartStageForPlayer - Speaker assigned: %s"), *currentSpeaker->GetPlayerName());

	// 서버에서도 첫 질문 Toast 표시 (OnRep는 클라이언트만 호출되므로)
	ShowCurrentQuestionToast();
}

void ASpeakStageActor::ServerRPC_RequestSpeak_Implementation(APlayerState* Player)
{
	if (!HasAuthority() || !Player)
	{
		return;
	}

	// 현재 발화자가 아니면 거부
	if (currentSpeaker != Player)
	{
		PRINTLOG(TEXT("[SpeakStage] RequestSpeak - Not your turn: %s (Current: %s)"),
		         *Player->GetPlayerName(),
		         currentSpeaker ? *currentSpeaker->GetPlayerName() : TEXT("None"));
		return;
	}

	// 발화 권한 승인
	PRINTLOG(TEXT("[SpeakStage] Speak permission granted: %s (Step: %d/%d)"),
	         *Player->GetPlayerName(), currentStepIndex + 1, totalQuestions);

	// 여기서 추가 처리 가능 (STT 시작 등)
	// 필요시 ClientRPC로 승인 알림 전송 가능
}

bool ASpeakStageActor::ServerRPC_RequestSpeak_Validate(APlayerState* Player)
{
	return Player != nullptr;
}

void ASpeakStageActor::ServerRPC_NotifyAnswerComplete_Implementation(APlayerState* Player)
{
	if (!HasAuthority() || !Player)
	{
		return;
	}

	// 현재 발화자가 아니면 무시
	if (currentSpeaker != Player)
	{
		PRINTLOG(TEXT("[SpeakStage] NotifyAnswerComplete - Invalid speaker: %s"), *Player->GetPlayerName());
		return;
	}

	PRINTLOG(TEXT("[SpeakStage] Answer complete: %s (Step: %d/%d)"),
	         *Player->GetPlayerName(), currentStepIndex + 1, totalQuestions);

	// 다음 단계로 진행
	AdvanceStep();
}

FString ASpeakStageActor::GetCurrentQuestion() const
{
	if (questions.IsValidIndex(currentStepIndex))
	{
		return questions[currentStepIndex];
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

	currentStepIndex++;

	// 모든 질문 완료?
	if (currentStepIndex >= totalQuestions)
	{
		PRINTLOG(TEXT("[SpeakStage] All steps completed for: %s"), *currentSpeaker->GetPlayerName());

		// 스테이지 종료
		EndStage();
	}
	else
	{
		PRINTLOG(TEXT("[SpeakStage] Advanced to step: %d/%d"), currentStepIndex + 1, totalQuestions);

		// 서버에서도 Toast 표시 (OnRep는 클라이언트만 호출됨)
		ShowCurrentQuestionToast();

		// RepNotify로 모든 클라이언트에 자동 전파
	}
}

void ASpeakStageActor::EndStage()
{
	if (!HasAuthority())
	{
		return;
	}

	if (!currentSpeaker)
	{
		return;
	}
	
	PRINTLOG(TEXT("[SpeakStage] Player completed and stage is ending for: %s"), *currentSpeaker->GetPlayerName());

	// 모든 단계 완료 Toast 표시
	if (UDialogManager* DM = UDialogManager::Get(GetWorld()))
	{
		DM->ShowToast(TEXT("🎉 입국 심사가 완료되었습니다!"));
		PRINTLOG(TEXT("[SpeakStage] Completion toast displayed"));
	}
	
	// 현재 발화자 초기화, 스테이지를 다시 사용 가능하게 만듦
	currentSpeaker = nullptr;
	currentStepIndex = 0;

	// 서버 측 리스너에게 즉시 알림
	OnSpeakerChanged.Broadcast(currentSpeaker);

	// 여기서 완료 이벤트 브로드캐스트 가능
	// 예: OnStageCompleted.Broadcast();
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
		currentStepIndex + 1,
		totalQuestions,
		*CurrentQuestion);

	// DialogManager를 통해 Toast 표시
	if (UDialogManager* DM = UDialogManager::Get(GetWorld()))
	{
		DM->ShowToast(ToastMessage);

		UGameSoundManager::Get(GetWorld())->PlaySound2D( questions_Voice[currentStepIndex] );
		
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
	questions.Empty();
	questions.Add(TEXT("What is your name?"));
	questions.Add(TEXT("Where are you from?"));
	questions.Add(TEXT("What is the purpose of your visit?"));
	questions.Add(TEXT("How long will you stay?"));
	questions.Add(TEXT("Where will you be staying?"));

	questions_Voice.Empty();
	questions_Voice.Add(EGameSoundType::What_is_your_name);
	questions_Voice.Add(EGameSoundType::Where_are_you_from);
	questions_Voice.Add(EGameSoundType::What_is_the_purpose_of_your_visit);
	questions_Voice.Add(EGameSoundType::How_long_will_you_stay);
	questions_Voice.Add(EGameSoundType::Where_will_you_be_staying);

	
	totalQuestions = questions.Num();

	PRINTLOG(TEXT("[SpeakStage] Test scenario data created: %d questions"), totalQuestions);
}
