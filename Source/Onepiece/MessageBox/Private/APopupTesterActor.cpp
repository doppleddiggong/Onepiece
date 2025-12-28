// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.


#include "APopupTesterActor.h"

#include "EPopupType.h"
#include "UDialogManager.h"
#include "UPopupManager.h"
#include "UPopup_MsgBox.h"

#include "GameLogging.h"
#include "ULingoGameHelper.h"
#include "UPopup_InputMsg.h"
#include "UPopup_ReadQuest.h"
#include "UPopup_Interview.h"
#include "NetworkData.h"
#include "Popup_Result.h"
#include "UCommonFunctionLibrary.h"
#include "UPopup_DailyStudy.h"
#include "UPopup_Evaluation.h"
#include "UPopup_InterviewHello.h"
#include "UPopup_LevelSelect.h"
#include "UPopup_History.h"


APopupTesterActor::APopupTesterActor()
{
	PrimaryActorTick.bCanEverTick = false;
}

void APopupTesterActor::GererateMD5()
{
	PRINT_STRING(TEXT("%s"), *UCommonFunctionLibrary::GererateMD5(MD5));
}

void APopupTesterActor::ToastMsg()
{
	UDialogManager::Get(GetWorld())->ShowToast(TEXT("토스트 메세지 샘플"));
}

void APopupTesterActor::OKCancelMsgBox()
{
	UPopupManager::Get(GetWorld())->ShowMsgBox(TEXT("알림"), TEXT("OK_CANCEL 설명문"),
		EMsgBoxType::OK_CANCEL,
		FOnMsgBoxOkDelegate::CreateUObject(this, &APopupTesterActor::OnOK),
		FOnMsgBoxCancelDelegate::CreateUObject(this, &APopupTesterActor::OnCancel));
}

void APopupTesterActor::OKMsgBox()
{
	UPopupManager::Get(GetWorld())->ShowMsgBox(TEXT("알림"), TEXT("OK 설명문"),
		EMsgBoxType::OK,
		FOnMsgBoxOkDelegate::CreateUObject(this, &APopupTesterActor::OnOK));
}

void APopupTesterActor::PopupInputMsg()
{
	if (auto Popup = UPopupManager::ShowPopupAs<UPopup_InputMsg>(GetWorld(), EPopupType::Login))
	{
		Popup->InitPopup(EPopupType::Login);
	}
}

void APopupTesterActor::ReadQuest()
{
	if (auto Popup = UPopupManager::ShowPopupAs<UPopup_ReadQuest>(GetWorld(), EPopupType::ReadQuest))
	{
		Popup->InitRead(ULingoGameHelper::GetLingoGameState(GetWorld())->ReadScenarioData);
	}
}

void APopupTesterActor::InterviewPopup()
{
	if (auto Popup = UPopupManager::ShowPopupAs<UPopup_InterviewHello>(GetWorld(), EPopupType::InterviewHello))
	{
		// 테스트용 더미 데이터 생성
			FResponseInterviewHello TestData;

			// 질문 1
			FInterviewQuestionData Q1;
			Q1.Id = 1;
			Q1.TypeCode = 0;
			Q1.Eng = TEXT("What is your current country of residence?");
			Q1.Kor = TEXT("현재 거주 국가는 어디인가요?");
			Q1.EngKey = TEXT("country");
			Q1.KorKey = TEXT("국가");
			Q1.CreatedAt = TEXT("2025-01-15");
			TestData.Questions.Add(Q1);

			// 질문 2
			FInterviewQuestionData Q2;
			Q2.Id = 2;
			Q2.TypeCode = 0;
			Q2.Eng = TEXT("How did you find out about this program?");
			Q2.Kor = TEXT("이 프로그램을 어떻게 알게 되었나요?");
			Q2.EngKey = TEXT("discovery");
			Q2.KorKey = TEXT("발견경로");
			Q2.CreatedAt = TEXT("2025-01-15");
			TestData.Questions.Add(Q2);

			// 질문 3
			FInterviewQuestionData Q3;
			Q3.Id = 3;
			Q3.TypeCode = 0;
			Q3.Eng = TEXT("What are your goals for learning Korean?");
			Q3.Kor = TEXT("한국어 학습의 목표는 무엇인가요?");
			Q3.EngKey = TEXT("goals");
			Q3.KorKey = TEXT("목표");
			Q3.CreatedAt = TEXT("2025-01-15");
			TestData.Questions.Add(Q3);

			// 팝업 초기화
			Popup->InitPopup(TestData);

			PRINTLOG(TEXT("[PopupTester] Interview popup opened with %d questions"), TestData.Questions.Num());
	}
}

void APopupTesterActor::TestEvaluationPopup()
{
	// 테스트 데이터 생성
	FResponseEvaluationResult TestData;

	// Total Result 설정
	TestData.total_result.final_score = 62;
	TestData.total_result.grade = TEXT("D");
	TestData.total_result.feedback_summary = TEXT("Your strengths lie in Reading and Listening, but there is a need for significant improvement in Speaking. How can you enhance your speaking skills to better articulate your thoughts?");

	// Scenario Results 설정
	FScenarioResult ReadingResult;
	ReadingResult.scenario_type = EScenarioType::READING;
	ReadingResult.display_name = TEXT("읽기 영역");
	ReadingResult.final_score = 93;
	ReadingResult.grade = TEXT("A");
	ReadingResult.feedback_summary.title = TEXT("Excellent Reading Skills");
	ReadingResult.feedback_summary.message = TEXT("Your reading comprehension is outstanding, demonstrating a strong grasp of the material.");
	ReadingResult.action_item = TEXT("Continue practicing by reading a variety of texts to maintain your high level of comprehension.");
	TestData.scenario_results.Add(ReadingResult);

	FScenarioResult ListeningResult;
	ListeningResult.scenario_type = EScenarioType::LISTENING;
	ListeningResult.display_name = TEXT("듣기 영역");
	ListeningResult.final_score = 79;
	ListeningResult.grade = TEXT("C");
	ListeningResult.feedback_summary.title = TEXT("Good Listening Skills");
	ListeningResult.feedback_summary.message = TEXT("You have a good ability to understand spoken English, but there is room for improvement in capturing finer details.");
	ListeningResult.action_item = TEXT("Listen to English podcasts or watch English shows to improve your listening skills.");
	TestData.scenario_results.Add(ListeningResult);

	FScenarioResult WritingResult;
	WritingResult.scenario_type = EScenarioType::WRITING;
	WritingResult.display_name = TEXT("쓰기 영역");
	WritingResult.final_score = 58;
	WritingResult.grade = TEXT("D");
	WritingResult.feedback_summary.title = TEXT("Writing Needs Improvement");
	WritingResult.feedback_summary.message = TEXT("Your writing lacks clarity and coherence, which affects the overall quality of your work.");
	WritingResult.action_item = TEXT("Practice writing short essays on various topics to improve structure and clarity.");
	TestData.scenario_results.Add(WritingResult);

	FScenarioResult SpeakingResult;
	SpeakingResult.scenario_type = EScenarioType::SPEAKING;
	SpeakingResult.display_name = TEXT("말하기 영역");
	SpeakingResult.final_score = 58;
	SpeakingResult.grade = TEXT("D");
	SpeakingResult.feedback_summary.title = TEXT("Speaking Needs Improvement");
	SpeakingResult.feedback_summary.message = TEXT("Your responses often lack relevance to the topic, affecting the flow and context of your speech.");
	SpeakingResult.action_item = TEXT("Engage in speaking exercises, such as role-playing or conversation practice, to stay on topic and improve relevance.");
	TestData.scenario_results.Add(SpeakingResult);

	// 팝업 표시
	if (auto Popup = UPopupManager::ShowPopupAs<UPopup_Evaluation>( GetWorld(), EPopupType::Evaluation))
		Popup->InitPopup(TestData);
}

void APopupTesterActor::PopupLevelSelect()
{
	if (auto Popup = UPopupManager::ShowPopupAs<UPopup_LevelSelect>(GetWorld(), EPopupType::LevelSelect))
	{
		Popup->InitPopup();
		PRINTLOG(TEXT("[PopupTester] Level Select popup opened"));
	}
}

void APopupTesterActor::TestPopupHistory()
{
	if (auto Popup = UPopupManager::ShowPopupAs<UPopup_History>(GetWorld(), EPopupType::History))
	{
		Popup->InitPopup();
		PRINTLOG(TEXT("[PopupTester] Chat History popup opened"));
	}
}

void APopupTesterActor::OnOK()
{
	PRINT_STRING(TEXT("I'M OK"));
}

void APopupTesterActor::OnCancel()
{
	PRINT_STRING(TEXT("I'M Cancel"));
}


