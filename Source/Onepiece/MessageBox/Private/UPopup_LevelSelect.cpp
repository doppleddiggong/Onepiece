// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.

#include "UPopup_LevelSelect.h"
#include "ULevelSelectItem.h"
#include "UPopupManager.h"
#include "UTextureButton.h"
#include "Components/HorizontalBox.h"
#include "Components/Button.h"
#include "Components/Spacer.h"

void UPopup_LevelSelect::NativeConstruct()
{
	Super::NativeConstruct();


}

void UPopup_LevelSelect::InitPopup()
{
	if (Btn_Close)
	{
		Btn_Close->OnButtonClickedEvent.RemoveDynamic(this, &UPopup_LevelSelect::OnClickClose);
		Btn_Close->OnButtonClickedEvent.AddDynamic(this, &UPopup_LevelSelect::OnClickClose);
	}
	
	// 기존 아이템들 제거
	HorizontalBox->ClearChildren();

	// 레벨 아이템들 생성 및 추가
	for (int32 i = 0; i < LevelNames.Num(); ++i)
	{
		ULevelSelectItem* LevelItem = CreateWidget<ULevelSelectItem>(this, LevelSelectItemClass);
		if (LevelItem)
		{
			// 레벨은 1부터 시작 (i+1)
			int32 Level = i + 1;
			FString LevelName = (i < LevelNames.Num()) ? LevelNames[i] : FString::Printf(TEXT("Level %d"), Level);

			LevelItem->InitLevelItem(Level, LevelName);

			// 델리게이트 바인딩
			LevelItem->OnLevelSelected.BindUObject(this, &UPopup_LevelSelect::OnLevelSelected);

			HorizontalBox->AddChildToHorizontalBox(LevelItem);
		}
		
		// 마지막 항목이 아니면 Spacer 추가
		if (i < LevelNames.Num() - 1)
		{
			USpacer* Spacer = NewObject<USpacer>(this);
			if (Spacer)
			{
				Spacer->SetSize(FVector2D(45.0f, 0));
				HorizontalBox->AddChildToHorizontalBox(Spacer);
			}
		}
	}
}

void UPopup_LevelSelect::OnLevelSelected(int32 SelectedLevel, const FString& SelectedLevelName)
{
	// 선택된 레벨 정보를 MessageBox로 표시
	FString Message = FString::Printf(TEXT("Selected Level: %d\nLevel Name: %s"), SelectedLevel, *SelectedLevelName);

	UPopupManager::Get(GetWorld())->ShowMsgBox(
		TEXT("Level Selected"),
		Message,
		EMsgBoxType::OK,
		FOnMsgBoxOkDelegate::CreateLambda([this]()
		{
			// MessageBox 확인 후 팝업 닫기
			OnClickClose();
		})
	);
}

void UPopup_LevelSelect::OnClickClose()
{
	// PopupManager를 통해 팝업 닫기
	if (UPopupManager* PopupMgr = UPopupManager::Get(GetWorld()))
	{
		PopupMgr->HideCurrentPopup();
	}
}