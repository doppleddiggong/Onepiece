// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.


#include "QuestionnaireKiosk.h"

#include "GameLogging.h"
#include "UInteractWidget.h"
#include "InteractableComponent.h"
#include "Popup_Questionnaire.h"
#include "UKLingoNetworkSystem.h"
#include "UPopupManager.h"
#include "UPopup_MsgBox.h"
#include "Components/BoxComponent.h"
#include "Components/WidgetComponent.h"

#define INTERACT_WIDGET_PATH TEXT("/Game/CustomContents/UI/Widgets/WBP_InteractWidget.WBP_InteractWidget_C")

class UInteractWidget;
// Sets default values
AQuestionnaireKiosk::AQuestionnaireKiosk()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	RootSceneComp = CreateDefaultSubobject<USceneComponent>(TEXT("RootSceneComp"));
	SetRootComponent(RootSceneComp);
	
	KioskMeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("KioskMeshComp"));
	KioskMeshComp->SetupAttachment(RootComponent);
	
	InteractableComp = CreateDefaultSubobject<UInteractableComponent>(TEXT("InteractableComp"));
	InteractableComp->InteractionType = EInteractionType::Button;
	InteractableComp->InteractionPrompt = TEXT("Activate");
	
	BoxComp = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComp"));
	BoxComp->SetupAttachment(GetRootComponent());
	BoxComp->SetRelativeLocation(FVector(0, 36, 75));
	BoxComp->SetBoxExtent(FVector(60, 65, 92));
	
	WidgetComp = CreateDefaultSubobject<UWidgetComponent>(TEXT("WidgetComp"));
	ConstructorHelpers::FClassFinder<UInteractWidget> WidgetRef(INTERACT_WIDGET_PATH);
	if (WidgetRef.Succeeded())
	{
		WidgetComp->SetWidgetClass(WidgetRef.Class);
		WidgetComp->SetupAttachment(GetRootComponent());
		WidgetComp->SetWidgetSpace(EWidgetSpace::Screen);
		WidgetComp->SetDrawSize(FVector2D(2048.0f, 1024.0f));
	}
}

void AQuestionnaireKiosk::BeginPlay()
{
	Super::BeginPlay();
	
	// 델리게이트 바인딩
	InteractableComp->InitWidget(WidgetComp);
	InteractableComp->OnInteractionTriggered.AddDynamic(this, &AQuestionnaireKiosk::OnInteractionTriggered);
	InteractableComp->OnOutlineStateChanged.AddDynamic(this, &AQuestionnaireKiosk::OnOutlineStateChanged);
}

void AQuestionnaireKiosk::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}

void AQuestionnaireKiosk::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AQuestionnaireKiosk::OnInteractionTriggered(AActor* Interactor)
{
	ServerRPC_OnInteractionTriggered(Interactor);
}

void AQuestionnaireKiosk::ServerRPC_OnInteractionTriggered_Implementation(AActor* Interactor)
{
	ClientRPC_OnInteractionTriggered(Interactor);
}

void AQuestionnaireKiosk::ClientRPC_OnInteractionTriggered_Implementation(AActor* Interactor)
{
	if (auto KLingoNetwork = UKLingoNetworkSystem::Get(GetWorld()))
	{
		// QuestionnaireData에 데이터가 있다면
		if (QuestionnaireData.IsValid())
		{
			PRINTLOG(TEXT("QuestionnaireData is valid"));
			ShowPopup();
		}
		else
		{
			PRINTLOG(TEXT("QuestionnaireData is invalid"));
			KLingoNetwork->RequestWriteQuestions(FResponseWriteQuestionDelegate::CreateUObject(this, &AQuestionnaireKiosk::OnResponseData));
		}
	}
}

void AQuestionnaireKiosk::OnResponseData(FQuestWriteInfo& InResponseData, bool bWasSuccessful)
{
	if (bWasSuccessful)
	{
		PRINTLOG(TEXT("--- Write Question Response SUCCESS ---"));
		
		// 쓰기 퀘스트 json 데이터 받기 요청
		QuestionnaireData = InResponseData;
		
		for (int32 i = 1; i <= QuestionnaireData.question.Num(); ++i)
		{
			FWriteQuestionData& data = QuestionnaireData.question[i - 1];
			data.Id = i;
			PRINTLOG(TEXT("%d kor: %s"), i, *data.word_data.kor);
			PRINTLOG(TEXT("%d answer_kor: %s"), i, *data.answer_kor);
		}
			
		// 테스트용 더미 데이터 생성
		// dCreateTestData(QuestionnaireData);
		
		ShowPopup();
	}
	else
	{
		PRINTLOG(TEXT("--- Write Question Response FAILED ---"));
	}
}

void AQuestionnaireKiosk::ShowPopup()
{
	if (auto Popup = UPopupManager::ShowPopupAs<UPopup_Questionnaire>(GetWorld(), EPopupType::Questionnaire))
	{
		// 팝업 초기화
		Popup->InitPopup(QuestionnaireData);
		
		PRINTLOG(TEXT("[PopupTester] Interview popup opened with %d questions"), QuestionnaireData.question.Num());
	}
}


void AQuestionnaireKiosk::CreateTestData(FQuestWriteInfo& TestData)
{
	// 질문 1
	FWriteQuestionData Q1;
	Q1.Id = 1;
	Q1.word_data.kor = TEXT("밤 몇 시에 잡니까?");
	Q1.word_data.eng = TEXT("When u sleep?");
	Q1.word_data.pronunciation = TEXT("bam myeot sie japnikka");
	Q1.answer = TEXT("23:00");
	Q1.answer_kor = TEXT("저는 밤 11시에 잡니다.");
	TestData.question.Add(Q1);

	// 질문 2
	FWriteQuestionData Q2;
	Q2.Id = 2;
	Q2.word_data.kor = TEXT("가족이 모두 몇 명입니까?");
	Q2.word_data.eng = TEXT("How many people are there in your family?");
	Q2.word_data.pronunciation = TEXT("gajogi modu myeot myeongipnikka");
	Q2.answer = TEXT("4");
	Q2.answer_kor = TEXT("가족은 모두 4명입니다.");
	TestData.question.Add(Q2);

	// 질문 3
	FWriteQuestionData Q3;
	Q3.Id = 3;
	Q3.word_data.kor = TEXT("오늘은 며칠입니까?");
	Q3.word_data.eng = TEXT("What's today's date?");
	Q3.word_data.pronunciation = TEXT("oneureun myeochiripnikka");
	Q3.answer = FDateTime::Now().ToString(TEXT("%Y-%m-%d"));
	Q3.answer_kor = FDateTime::Now().ToString(TEXT("오늘은 %Y년 %m월 %d일입니다."));
	TestData.question.Add(Q3);
			
	// 질문 4
	FWriteQuestionData Q4;
	Q4.Id = 4;
	Q4.word_data.kor = TEXT("오늘 할 게임은 무엇입니까?");
	Q4.word_data.eng = TEXT("What game are you going to play today?");
	Q4.word_data.pronunciation = TEXT("oneul hal geimeun mueosipnikka?");
	Q4.answer = FDateTime::Now().ToString(TEXT("Peak"));
	Q4.answer_kor = FDateTime::Now().ToString(TEXT("오늘은 Peak라는 게임을 할 것입니다."));
	TestData.question.Add(Q4);
	
	TestData.bIsValid = true;
}

void AQuestionnaireKiosk::OnOutlineStateChanged(bool bShouldShowOutline)
{
	if (KioskMeshComp)
	{
		KioskMeshComp->SetRenderCustomDepth(bShouldShowOutline);
	}
}
