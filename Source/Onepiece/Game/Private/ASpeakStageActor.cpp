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

void ASpeakStageActor::OnRep_CurrentStepIndex()
{
	// 클라이언트에서 Toast 표시
	ShowCurrentQuestionToast();
}

//----------------------------------------------------------
// Public Interface
//----------------------------------------------------------

void ASpeakStageActor::StartStageForPlayer(ALingoPlayerState* Player)
{
	if (!HasAuthority())
		return;

	if (!Player)
		return;

	// 이미 사용 중인 경우
	if (CurrentSpeaker != nullptr)
		return;
	
	// 플레이어를 현재 발화자로 설정
	CurrentSpeaker = Player;
	CurrentStepIndex = 0;

	// 서버 측 리스너에게 즉시 알림
	OnSpeakerChanged.Broadcast(CurrentSpeaker);

	// 서버에서도 첫 질문 Toast 표시 (OnRep는 클라이언트만 호출되므로)
	ShowCurrentQuestionToast();

	// 첫 번째 질문 TTS 재생 및 UI 업데이트
	if (ALingoPlayerState* PS = Cast<ALingoPlayerState>(CurrentSpeaker))
	{
		if (PS->SpeakScenarioData.speak_quest_data.IsValidIndex(CurrentStepIndex))
		{
			if (APawn* SpeakerPawn = CurrentSpeaker->GetPawn())
			{
				if (APlayerActor* PlayerActor = Cast<APlayerActor>(SpeakerPawn))
				{
					// TTS 재생
					FSpeakStageQuestion& CurrentQuestion = PS->SpeakScenarioData.speak_quest_data[CurrentStepIndex];
					PlayerActor->PlayTTSAudio(CurrentQuestion.voice_data);
					PRINTLOG(TEXT("[SpeakStage] Playing audio for step %d"), CurrentStepIndex + 1);

					// UI 업데이트 (PlayerControl을 통해)
					if (APlayerControl* PC = Cast<APlayerControl>(PlayerActor->GetController()))
					{
						PC->Client_UpdateSpeakWidget();
						PRINTLOG(TEXT("[SpeakStage] SpeakWidget UI update requested"));
					}
				}
			}
		}
	}
}

void ASpeakStageActor::ServerRPC_NotifyAnswerComplete_Implementation(ALingoPlayerState* Player)
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
		return  PS->SpeakScenarioData.speak_quest_data.Num();
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

		// 서버에서도 Toast 표시 (OnRep는 클라이언트만 호출됨)
		ShowCurrentQuestionToast();

		// 현재 발화자에게 다음 질문 음성 재생 및 UI 업데이트
		if (ALingoPlayerState* PS = Cast<ALingoPlayerState>(CurrentSpeaker))
		{
			if (PS->SpeakScenarioData.speak_quest_data.IsValidIndex(CurrentStepIndex))
			{
				if (APawn* SpeakerPawn = CurrentSpeaker->GetPawn())
				{
					if (APlayerActor* PlayerActor = Cast<APlayerActor>(SpeakerPawn))
					{
						// TTS 재생
						FSpeakStageQuestion& CurrentQuestion = PS->SpeakScenarioData.speak_quest_data[CurrentStepIndex];
						PlayerActor->PlayTTSAudio(CurrentQuestion.voice_data);
						PRINTLOG(TEXT("[SpeakStage] Playing audio for step %d"), CurrentStepIndex + 1);

						// UI 업데이트 (PlayerControl을 통해)
						if (APlayerControl* PC = Cast<APlayerControl>(PlayerActor->GetController()))
						{
							PC->Client_UpdateSpeakWidget();
						}
					}
				}
			}
		}
	}
}

void ASpeakStageActor::EndStage()
{
	if (!HasAuthority())
		return;

	if (!CurrentSpeaker)
		return;

	// 모든 단계 완료 Toast 표시
	UPopupManager::Get(GetWorld())->ShowMsgBox(TEXT("NOTICE"), TEXT("SPEAK QUEST COMPLETE"),
		EMsgBoxType::OK, FOnMsgBoxOkDelegate());

	// UI 업데이트를 위해 현재 발화자 임시 저장
	APlayerState* PreviousSpeaker = CurrentSpeaker;
	
	// 현재 발화자 초기화, 스테이지를 다시 사용 가능하게 만듦
	CurrentSpeaker = nullptr;
	CurrentStepIndex = 0;

	// 서버 측 리스너에게 즉시 알림
	OnSpeakerChanged.Broadcast(CurrentSpeaker);

	// UI 업데이트 요청 (CurrentSpeaker가 nullptr이 된 후 위젯 숨김)
	if (APawn* SpeakerPawn = PreviousSpeaker->GetPawn())
	{
		if (APlayerActor* PlayerActor = Cast<APlayerActor>(SpeakerPawn))
		{
			if (APlayerControl* PC = Cast<APlayerControl>(PlayerActor->GetController()))
			{
				PC->Client_UpdateSpeakWidget();
				PRINTLOG(TEXT("[SpeakStage] SpeakWidget hidden after quest completion"));
			}
		}
	}
}

void ASpeakStageActor::ShowCurrentQuestionToast()
{
	// 현재 질문 가져오기
	FString CurrentQuestion = GetCurrentQuestion();
	if (CurrentQuestion.IsEmpty())
		return;

	// PlayerState에서 전체 질문 수 가져오기
	auto TotalQuestionsCount = GetTotalQuestionsCount();

	// Toast 메시지 생성
	FString ToastMessage = FString::Printf(TEXT("[%d/%d] %s"),
		CurrentStepIndex + 1,
		TotalQuestionsCount,
		*CurrentQuestion);

	// DialogManager를 통해 Toast 표시
	if (UDialogManager* DM = UDialogManager::Get(GetWorld()))
	{
		DM->ShowToast(ToastMessage);
		PRINTLOG(TEXT("[SpeakStage] Toast displayed: %s"), *ToastMessage);
	}
}