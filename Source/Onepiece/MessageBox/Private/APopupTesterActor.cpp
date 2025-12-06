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
		Popup->InitPopup(ULingoGameHelper::GetLingoGameState(GetWorld())->CurScenarioData);
	}
}

void APopupTesterActor::InterviewPopup()
{
	if (auto Popup = UPopupManager::ShowPopupAs<UPopup_Interview>(GetWorld(), EPopupType::Interview))
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

void APopupTesterActor::ResultPopup()
{
	if (const auto PopupMgr = UPopupManager::Get(GetWorld()))
	{
		PopupMgr->ShowResult();
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


