// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.

#include "ADailyKiosk.h"

#include "GameLogging.h"
#include "UInteractWidget.h"
#include "InteractableComponent.h"
#include "UKLingoNetworkSystem.h"
#include "UPopupManager.h"
#include "UPopup_DailyStudy.h"
#include "UGameDataManager.h"
#include "UCommonFunctionLibrary.h"
#include "Components/BoxComponent.h"
#include "Components/WidgetComponent.h"
#include "Onepiece/Onepiece.h"

#define DAILYKIOSK_INTERACT_WIDGET_PATH TEXT("/Game/CustomContents/UI/Widgets/WBP_InteractWidget_Daily.WBP_InteractWidget_Daily_C")

ADailyKiosk::ADailyKiosk()
{
	PrimaryActorTick.bCanEverTick = false;

	RootSceneComp = CreateDefaultSubobject<USceneComponent>(TEXT("RootSceneComp"));
	SetRootComponent(RootSceneComp);

	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	MeshComp->SetupAttachment(RootComponent);

	InteractableComp = CreateDefaultSubobject<UInteractableComponent>(TEXT("Interactable"));
	InteractableComp->InteractionType = EInteractionType::Kiosk;
	InteractableComp->InteractionPrompt = TEXT("Activate");

	BoxComp = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComp"));
	BoxComp->SetupAttachment(GetRootComponent());
	BoxComp->SetRelativeLocation(FVector(0, 36, 75));
	BoxComp->SetBoxExtent(FVector(60, 65, 92));

	WidgetComp = CreateDefaultSubobject<UWidgetComponent>(TEXT("WidgetComp"));
	ConstructorHelpers::FClassFinder<UInteractWidget> WidgetRef(DAILYKIOSK_INTERACT_WIDGET_PATH);
	if (WidgetRef.Succeeded())
	{
		WidgetComp->SetWidgetClass(WidgetRef.Class);
		WidgetComp->SetupAttachment(GetRootComponent());
		WidgetComp->SetWidgetSpace(EWidgetSpace::Screen);
		WidgetComp->SetDrawSize(FVector2D(2048.0f, 1024.0f));
	}
}

void ADailyKiosk::BeginPlay()
{
	Super::BeginPlay();

	// 델리게이트 바인딩
	InteractableComp->InitWidget(WidgetComp);
	InteractableComp->OnInteractionTriggered.AddDynamic(this, &ADailyKiosk::OnInteractionTriggered);
	InteractableComp->OnOutlineStateChanged.AddDynamic(this, &ADailyKiosk::OnOutlineStateChanged);
}

void ADailyKiosk::OnInteractionTriggered(AActor* Interactor)
{
	// ChatDaily API 요청
	if (auto KLingoNetwork = UKLingoNetworkSystem::Get(GetWorld()))
	{
		PRINTLOG(TEXT("[DailyKiosk] Requesting daily question..."));
		KLingoNetwork->RequestDailyQuestion(
			DefineData::DailySystemPrompt,
			DefineData::DailyUserPrompt,
			FResponseChatDailysDelegate::CreateUObject(this, &ADailyKiosk::OnResponseDailyQuestion)
		);
	}
}

void ADailyKiosk::OnResponseDailyQuestion(FResponseChatDailys& InResponseData, bool bWasSuccessful)
{
	if (!bWasSuccessful)
	{
		PRINTLOG(TEXT("[DailyKiosk] Network request failed. Using fallback word data."));

		// 네트워크 실패 시 랜덤 단어 데이터로 대체
		TArray<FWordData> FallbackWordData = GetRandomKoreanWords(DefineData::MIN_REQUIRED_WORDS);

		if (FallbackWordData.Num() > 0)
		{
			if (UPopup_DailyStudy* DailyStudyPopup = UPopupManager::Get(GetWorld())->ShowPopupAs<UPopup_DailyStudy>(EPopupType::DailyStudy))
			{
				DailyStudyPopup->InitPopup(FallbackWordData);
			}
		}
		else
		{
			PRINTLOG(TEXT("[DailyKiosk] ERROR: No fallback data available!"));
		}

		return;
	}

	// AI 응답을 | 구분자로 파싱 (형식: "개|DOG|Gae|고양이|CAT|Go-yang-i")
	TArray<FString> RawTokens;
	InResponseData.answer.ParseIntoArray(RawTokens, TEXT("|"), true);

	// 3개씩 묶어서 FWordData로 변환 (Kor|Eng|Pronunciation)
	TArray<FWordData> ValidWordDataArray;
	for (int32 i = 0; i + 2 < RawTokens.Num(); i += 3)
	{
		FString Kor = RawTokens[i].TrimStartAndEnd();
		FString Eng = RawTokens[i + 1].TrimStartAndEnd();
		FString Pronunciation = RawTokens[i + 2].TrimStartAndEnd();

		// 빈 문자열 체크
		if (Kor.IsEmpty() || Eng.IsEmpty() || Pronunciation.IsEmpty())
		{
			PRINTLOG(TEXT("[DailyKiosk] Skipped: Empty field (Kor: '%s', Eng: '%s', Phon: '%s')"), *Kor, *Eng, *Pronunciation);
			continue;
		}

		// 한국어 검증 (Kor 필드만)
		if (!UCommonFunctionLibrary::IsValidKoreanWord(Kor))
		{
			PRINTLOG(TEXT("[DailyKiosk] Skipped: Invalid Korean word '%s'"), *Kor);
			continue;
		}

		// FWordData 생성
		FWordData WordData;
		WordData.Kor = Kor;
		WordData.Eng = Eng;
		WordData.Pronunciation = Pronunciation;

		ValidWordDataArray.Add(WordData);
		PRINTLOG(TEXT("[DailyKiosk] Parsed Word: Kor='%s', Eng='%s', Phon='%s'"), *Kor, *Eng, *Pronunciation);
	}

	// 유효한 단어가 부족하면 GameDataManager에서 보충
	if (ValidWordDataArray.Num() < DefineData::MIN_REQUIRED_WORDS)
	{
		int32 WordsNeeded = DefineData::MIN_REQUIRED_WORDS - ValidWordDataArray.Num();

		TArray<FWordData> AdditionalWordData = GetRandomKoreanWords(WordsNeeded);
		ValidWordDataArray.Append(AdditionalWordData);
	}

	// 최종 검증 및 팝업 표시
	if (ValidWordDataArray.Num() > 0)
	{
		if (UPopup_DailyStudy* DailyStudyPopup = UPopupManager::Get(GetWorld())->ShowPopupAs<UPopup_DailyStudy>(EPopupType::DailyStudy))
		{
			DailyStudyPopup->InitPopup(ValidWordDataArray);
			PRINTLOG(TEXT("[DailyKiosk] Initialized DailyStudy popup with %d words"), ValidWordDataArray.Num());
		}
	}
	else
	{
		PRINTLOG(TEXT("[DailyKiosk] ERROR: No valid word data!"));
	}
}

void ADailyKiosk::OnOutlineStateChanged(bool bShouldShowOutline)
{
	if (MeshComp)
	{
		MeshComp->SetRenderCustomDepth(bShouldShowOutline);
	}
}

TArray<FWordData> ADailyKiosk::GetRandomKoreanWords(int32 Count)
{
	TArray<FWordData> RandomWordDataArray;

	UGameDataManager* DataManager = UGameDataManager::Get(GetWorld());
	if (!DataManager)
	{
		PRINTLOG(TEXT("[DailyKiosk] Error: GameDataManager not found"));
		return RandomWordDataArray;
	}

	// ReadData에서 모든 키 가져오기
	TArray<int32> AllKeys = DataManager->GetAllReadDataKeys();

	if (AllKeys.Num() == 0)
	{
		PRINTLOG(TEXT("[DailyKiosk] Error: No ReadData available"));
		return RandomWordDataArray;
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
			// FWordData 생성
			FWordData WordData;
			WordData.Kor = ReadData.Word;
			WordData.Eng = ReadData.Eng;
			WordData.Pronunciation = ReadData.Eng; // TODO: 나중에 실제 발음 데이터로 교체

			RandomWordDataArray.Add(WordData);
		}
	}

	PRINTLOG(TEXT("[DailyKiosk] Generated %d random word data from ReadData"), RandomWordDataArray.Num());
	return RandomWordDataArray;
}