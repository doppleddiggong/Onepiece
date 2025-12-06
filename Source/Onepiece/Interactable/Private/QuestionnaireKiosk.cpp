// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.


#include "QuestionnaireKiosk.h"

#include "GameLogging.h"
#include "UInteractWidget.h"
#include "InteractableComponent.h"
#include "Popup_Questionnaire.h"
#include "UPopupManager.h"
#include "UPopup_Interview.h"
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
	// TODO: 쓰기 퀘스트 json 데이터 받기 요청
	
	if (auto Popup = UPopupManager::ShowPopupAs<UPopup_Questionnaire>(GetWorld(), EPopupType::Questionnaire))
		{
			// 테스트용 더미 데이터 생성
			FQuestWriteInfo TestData;

			// 질문 1
			FWriteQuestionData Q1;
			Q1.Id = 1;
			Q1.WordData.QuestionKr = TEXT("밤 몇 시에 잡니까?");
			Q1.WordData.QuestionEn = TEXT("When u sleep?");
			Q1.WordData.Pronunciation = TEXT("bam myeot sie japnikka");
			Q1.Answer = TEXT("23:00");
			Q1.AnswerKr = TEXT("저는 밤 11시에 잡니다.");
			TestData.Questions.Add(Q1);

			// 질문 2
			FWriteQuestionData Q2;
			Q2.Id = 2;
			Q2.WordData.QuestionKr = TEXT("가족이 모두 몇 명입니까?");
			Q2.WordData.QuestionEn = TEXT("How many people are there in your family?");
			Q2.WordData.Pronunciation = TEXT("gajogi modu myeot myeongipnikka");
			Q2.Answer = TEXT("4");
			Q2.AnswerKr = TEXT("가족은 모두 4명입니다.");
			TestData.Questions.Add(Q2);

			// 질문 3
			FWriteQuestionData Q3;
			Q3.Id = 3;
			Q3.WordData.QuestionKr = TEXT("오늘은 며칠입니까?");
			Q3.WordData.QuestionEn = TEXT("What's today's date?");
			Q3.WordData.Pronunciation = TEXT("oneureun myeochiripnikka");
			Q3.Answer = FDateTime::Now().ToString(TEXT("%Y-%m-%d"));
			Q3.AnswerKr = FDateTime::Now().ToString(TEXT("오늘은 %Y년 %m월 %d일입니다."));
			// TestData.Questions.Add(Q3);
			
			// 질문 4
			FWriteQuestionData Q4;
			Q3.Id = 4;
			Q3.WordData.QuestionKr = TEXT("오늘 할 게임은 무엇입니까?");
			Q3.WordData.QuestionEn = TEXT("What game are you going to play today?");
			Q3.WordData.Pronunciation = TEXT("oneul hal geimeun mueosipnikka?");
			Q3.Answer = FDateTime::Now().ToString(TEXT("Peak"));
			Q3.AnswerKr = FDateTime::Now().ToString(TEXT("오늘은 Peak라는 게임을 할 것입니다."));
			// TestData.Questions.Add(Q3);

			// 팝업 초기화
			Popup->InitPopup(TestData);

			PRINTLOG(TEXT("[PopupTester] Interview popup opened with %d questions"), TestData.Questions.Num());
	}
}