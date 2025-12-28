// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.


#include "ChatInputBox.h"

#include "APlayerControl.h"
#include "ChatWidget.h"
#include "GameLogging.h"
#include "UKLingoNetworkSystem.h"
#include "UPopupManager.h"
#include "UPopup_DailyStudy.h"
#include "UGameDataManager.h"
#include "UCommonFunctionLibrary.h"
#include "Components/Button.h"
#include "Components/MultiLineEditableTextBox.h"
#include "Framework/Application/SlateApplication.h"
#include "Onepiece/Onepiece.h"




void UChatInputBox::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	Button_Send->OnClicked.AddDynamic(this, &UChatInputBox::HandleSendClicked);
}

void UChatInputBox::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	// 현재 포커스 상태 확인
	bool bIsFocused = MultiLineEditableTextBox_Input && MultiLineEditableTextBox_Input->HasKeyboardFocus();

	// 포커스 상태 변화 감지
	if (bWasFocused != bIsFocused)
	{
		PRINTLOG(TEXT("[ChatInputBox] Focus changed: %d -> %d"), bWasFocused, bIsFocused);

		// 포커스를 잃었을 때만 처리 (화면 클릭 등)
		if (bWasFocused && !bIsFocused)
		{
			PRINTLOG(TEXT("[ChatInputBox] Focus lost - switching to GameOnly mode"));

			// GameOnly 모드로 전환
			if (APlayerControl* PC = Cast<APlayerControl>(GetOwningPlayer()))
			{
				FInputModeGameOnly InputMode;
				PC->SetInputMode(InputMode);
				PC->SetShowMouseCursor(false);
				FSlateApplication::Get().SetAllUserFocusToGameViewport();
			}

			// ChatWidget에 포커스 해제 알림
			if (OwningChatWidget)
			{
				PRINTLOG(TEXT("[ChatInputBox] Notifying ChatWidget of focus loss"));
				OwningChatWidget->OnInputFocusChanged(false);
			}
			else
			{
				PRINTLOG(TEXT("[ChatInputBox] ERROR: OwningChatWidget is null!"));
			}
		}
	}

	bWasFocused = bIsFocused;
}

FReply UChatInputBox::NativeOnPreviewKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
	// Enter 키 감지 & Shift+Enter는 줄바꿈 허용
	if (InKeyEvent.GetKey() == EKeys::Enter && !InKeyEvent.IsShiftDown())
	{
		HandleSendClicked();
		return FReply::Handled();
	}

	// ESC 키로 포커스 해제
	if (InKeyEvent.GetKey() == EKeys::Escape)
	{
		SetInputFocus(false);
		return FReply::Handled();
	}

	return Super::NativeOnPreviewKeyDown(InGeometry, InKeyEvent);
}

FText UChatInputBox::FlushMessage()
{
	// message에 저장 & 텍스트 칸 비우기
	FText message = MultiLineEditableTextBox_Input->GetText();
	
	MultiLineEditableTextBox_Input->SetText(FText::GetEmpty());
	
	return message;
}

void UChatInputBox::SetInputFocus(bool bFocus)
{
	if ( bFocus )
	{
		// 먼저 포커스 설정
		MultiLineEditableTextBox_Input->SetKeyboardFocus();

		// UI 입력 모드로 전환
		if (APlayerControl* PC = Cast<APlayerControl>(GetOwningPlayer()))
		{
			FInputModeUIOnly InputMode;
			InputMode.SetWidgetToFocus(MultiLineEditableTextBox_Input->TakeWidget());
			PC->SetInputMode(InputMode);
			PC->SetShowMouseCursor(true);
		}

		// ChatWidget에 포커스 획득 알림
		if (OwningChatWidget)
		{
			OwningChatWidget->OnInputFocusChanged(true);
		}

		bWasFocused = true;
	}
	else
	{
		// GameOnly 모드로 전환 및 포커스 해제
		if (APlayerControl* PC = Cast<APlayerControl>(GetOwningPlayer()))
		{
			FInputModeGameOnly InputMode;
			PC->SetInputMode(InputMode);
			PC->SetShowMouseCursor(false);

			// 뷰포트로 포커스 이동 (자동으로 입력창 포커스 해제됨)
			FSlateApplication::Get().SetAllUserFocusToGameViewport();
		}

		// ChatWidget에 포커스 해제 알림
		if (OwningChatWidget)
		{
			OwningChatWidget->OnInputFocusChanged(false);
		}

		bWasFocused = false;
	}
}

bool UChatInputBox::IsAIAsk(const FString& InMessage, FString& OutQuestion) const
{
	FString LeftPart, RightPart;

	// 콜론(:)을 기준으로 분리
	if (InMessage.Split(TEXT(":"), &LeftPart, &RightPart))
	{
		// 왼쪽 파트의 공백을 제거하고 "AI"와 일치하는지 확인 (대소문자 무시)
		if (LeftPart.TrimStartAndEnd().Equals(DefineData::AI, ESearchCase::IgnoreCase))
		{
			OutQuestion = RightPart.TrimStart();
			return !OutQuestion.IsEmpty(); // 내용이 비어있지 않아야 true
		}
	}
	return false;
}

bool UChatInputBox::IsDailyAsk(const FString& InMessage, FString& OutQuestion) const
{
	FString LeftPart, RightPart;

	if (InMessage.Split(TEXT(":"), &LeftPart, &RightPart))
	{
		if (LeftPart.TrimStartAndEnd().Equals(DefineData::Daily, ESearchCase::IgnoreCase))
		{
			OutQuestion = RightPart.TrimStart();
			return !OutQuestion.IsEmpty(); // 내용이 비어있지 않아야 true
		}
	}
	return false;
}

void UChatInputBox::HandleSendClicked()
{
	// 텍스트가 비어있지 않다면 처리
	FText Message = FlushMessage();
	if (Message.IsEmpty())
	{
		// 빈 메시지면 포커스만 해제
		SetInputFocus(false);
		return;
	}

	auto* PC = Cast<APlayerControl>(GetWorld()->GetFirstPlayerController());
	if (!PC)
		return;

	const FString MessageStr = Message.ToString();
	FString CleanQuestion;

	if (IsAIAsk(MessageStr, CleanQuestion))
	{
		// 사용자가 입력한 전체 메시지 표시 (로그성)
		PC->ServerRPC_SendChat(Message);

		// AI에게 정제된 질문 전송
		PC->ServerRPC_SendAIQuestion(CleanQuestion);

		PRINTLOG(TEXT("[AI Chat] User question: %s"), *CleanQuestion);
	}
	else if (IsDailyAsk(MessageStr, CleanQuestion))
	{
		// Daily 단어 생성 요청
		if (UKLingoNetworkSystem* NetworkSystem = UKLingoNetworkSystem::Get(GetWorld()))
		{
			CleanQuestion = TEXT("[Generate Korean words only] ") + CleanQuestion;
			
			NetworkSystem->RequestChatQuestion(DefineData::DailySystemPrompt, CleanQuestion,
				FResponseChatAnswersDelegate::CreateUObject(this, &UChatInputBox::OnDailyAnswerReceived));

			PRINTLOG(TEXT("[Daily] Word generation request: %s"), *CleanQuestion);
		}
	}
	else
	{
		// 2. 일반 채팅 메시지 전송
		PC->ServerRPC_SendChat(Message);
	}

	// 메시지 전송 후 포커스 해제 및 게임 모드로 복원
	SetInputFocus(false);

}

bool UChatInputBox::HasKeyboardFocus()
{
	return MultiLineEditableTextBox_Input && MultiLineEditableTextBox_Input->HasKeyboardFocus();
}

TArray<FString> UChatInputBox::GetRandomKoreanWords(int32 Count)
{
	TArray<FString> RandomWords;

	UGameDataManager* DataManager = UGameDataManager::Get(GetWorld());
	if (!DataManager)
	{
		PRINTLOG(TEXT("[Daily] Error: GameDataManager not found"));
		return RandomWords;
	}

	// ReadData에서 모든 키 가져오기
	TArray<int32> AllKeys = DataManager->GetAllReadDataKeys();
	
	if (AllKeys.Num() == 0)
	{
		PRINTLOG(TEXT("[Daily] Error: No ReadData available"));
		return RandomWords;
	}

	// 요청한 개수만큼 랜덤 단어 선택
	int32 WordsToGenerate = FMath::Min(Count, AllKeys.Num());
	
	for (int32 i = 0; i < WordsToGenerate; ++i)
	{
		// 랜덤 인덱스 선택
		int32 RandomIndex = FMath::RandRange(0, AllKeys.Num() - 1);
		int32 RandomKey = AllKeys[RandomIndex];
		AllKeys.RemoveAt(RandomIndex); // 중복 방지

		// 데이터 로드
		FReadData ReadData;
		if (DataManager->GetReadData(RandomKey, ReadData))
		{
			RandomWords.Add(ReadData.Word);
		}
	}

	PRINTLOG(TEXT("[Daily] Generated %d random words from ReadData"), RandomWords.Num());
	return RandomWords;
}

void UChatInputBox::OnDailyAnswerReceived(FResponseChatAnswers& ResponseData, bool bWasSuccessful)
{
	const int32 MIN_REQUIRED_WORDS = 3;

	if (!bWasSuccessful)
	{
		// 네트워크 실패 시 랜덤 단어로 대체
		TArray<FString> FallbackWords = GetRandomKoreanWords(MIN_REQUIRED_WORDS);
		
		if (FallbackWords.Num() > 0)
		{
			if (UPopup_DailyStudy* DailyStudyPopup = UPopupManager::Get(GetWorld())->ShowPopupAs<UPopup_DailyStudy>(EPopupType::DailyStudy))
			{
				DailyStudyPopup->InitPopup(FallbackWords);
			}
		}

		return;
	}

	// AI 응답을 | 구분자로 파싱
	TArray<FString> RawWords;
	ResponseData.answer.ParseIntoArray(RawWords, TEXT("|"), true);

	// 한국어 단어만 필터링
	TArray<FString> ValidKoreanWords;
	for (const FString& Word : RawWords)
	{
		FString TrimmedWord = Word.TrimStartAndEnd();
		
		// 빈 문자열 체크
		if (TrimmedWord.IsEmpty())
		{
			PRINTLOG(TEXT("[Daily] Skipped: Empty word"));
			continue;
		}

		// 한국어 검증
		if (!UCommonFunctionLibrary::IsValidKoreanWord(TrimmedWord))
		{
			PRINTLOG(TEXT("[Daily] Skipped: Non-Korean word '%s'"), *TrimmedWord);
			continue;
		}

		ValidKoreanWords.Add(TrimmedWord);
	}

	// 유효한 단어가 부족하면 GameDataManager에서 보충
	if (ValidKoreanWords.Num() < MIN_REQUIRED_WORDS)
	{
		int32 WordsNeeded = MIN_REQUIRED_WORDS - ValidKoreanWords.Num();

		TArray<FString> AdditionalWords = GetRandomKoreanWords(WordsNeeded);
		ValidKoreanWords.Append(AdditionalWords);
	}

	// 최종 검증
	if (ValidKoreanWords.Num() > 0)
	{
		if (UPopup_DailyStudy* DailyStudyPopup = UPopupManager::Get(GetWorld())->ShowPopupAs<UPopup_DailyStudy>(EPopupType::DailyStudy))
			DailyStudyPopup->InitPopup(ValidKoreanWords);
	}
}