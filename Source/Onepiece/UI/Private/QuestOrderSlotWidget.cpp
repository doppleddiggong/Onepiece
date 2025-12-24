// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.


#include "QuestOrderSlotWidget.h"

#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Components/WidgetSwitcher.h"
#include "Math/UnrealMathUtility.h"


UQuestOrderSlotWidget::UQuestOrderSlotWidget(FObjectInitializer const& ObjectInitializer) : Super(ObjectInitializer)
{
	ConstructorHelpers::FObjectFinder<UTexture2D> readQuestTextureRef(TEXT("/Script/Engine.Texture2D'/Game/CustomContents/UI/Texture/Resource/Icon_Read.Icon_Read'"));
	if (readQuestTextureRef.Succeeded())
	{
		ReadQuestTexture = readQuestTextureRef.Object;
	}
	ConstructorHelpers::FObjectFinder<UTexture2D> listenQuestTextureRef(TEXT("/Script/Engine.Texture2D'/Game/CustomContents/UI/Texture/Resource/Icon_Listen.Icon_Listen'"));
	if (listenQuestTextureRef.Succeeded())
	{
		ListenQuestTexture = listenQuestTextureRef.Object;
	}
	ConstructorHelpers::FObjectFinder<UTexture2D> speakQuestTextureRef(TEXT("/Script/Engine.Texture2D'/Game/CustomContents/UI/Texture/Resource/Icon_Speak.Icon_Speak'"));
	if (speakQuestTextureRef.Succeeded())
	{
		SpeakQuestTexture = speakQuestTextureRef.Object;
	}
	ConstructorHelpers::FObjectFinder<UTexture2D> writeQuestTextureRef(TEXT("/Script/Engine.Texture2D'/Game/CustomContents/UI/Texture/Resource/Icon_Write.Icon_Write'"));
	if (writeQuestTextureRef.Succeeded())
	{
		WriteQuestTexture = writeQuestTextureRef.Object;
	}
}

void UQuestOrderSlotWidget::SetQuestType(const char inQuestType)
{
	switch (inQuestType)
	{
		case 'R': Image_QuestType->SetBrushFromTexture(ReadQuestTexture); break;
		case 'L': Image_QuestType->SetBrushFromTexture(ListenQuestTexture); break;
		case 'S': Image_QuestType->SetBrushFromTexture(SpeakQuestTexture); break;
		case 'W': Image_QuestType->SetBrushFromTexture(WriteQuestTexture); break;
		default: break;
	}
}

void UQuestOrderSlotWidget::PlayQuestSlot()
{
	Image_Bg->SetColorAndOpacity(FColor::FromHex("00FF94FF"));
}

void UQuestOrderSlotWidget::FinishQuestSlot()
{
	Image_Bg->SetColorAndOpacity(FColor::FromHex("FFCA50FF"));
	WidgetSwitcher_Slot->SetActiveWidgetIndex(1);
	PlayStampAnimation(GetWorld()->DeltaTimeSeconds);
}

void UQuestOrderSlotWidget::PlayStampAnimation(float InDeltaTime)
{
	auto easeOutElastic = [](float x)->float
	{
		const float c4 = (2 * PI) / 3;
			
		if (x == 0)
		{
			return 0;
		}
		if (x == 1)
		{
			return 1;
		}
			
		return FMath::Pow(2, -10 * x) * FMath::Sin((x * 10 - 0.75) * c4) + 1;
	};
	
	GetWorld()->GetTimerManager().SetTimer(StampTimerHandle, [this, easeOutElastic, InDeltaTime]()
	{
		Image_Complete->SetRenderScale(FVector2D(StampStartScale - easeOutElastic(StampDX)));
		StampDX += InDeltaTime;
		if (StampDX >= 1.0f)
		{
			GetWorld()->GetTimerManager().ClearTimer(StampTimerHandle);
			StampDX = 0;
		}
	}, InDeltaTime, true);
}
