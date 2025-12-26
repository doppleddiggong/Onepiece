// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.

#include "UPopup_HowToPlay.h"
#include "UPageScrollView.h"
#include "UPageScrollItem.h"
#include "UPopup_HowToPlayItem.h"
#include "UPopupManager.h"
#include "UTextureButton.h"
#include "UGameDataManager.h"

void UPopup_HowToPlay::InitPopup(const TArray<EHowToPlayPageType>& InPageTypes )
{
	// 버튼 이벤트 바인딩
	if (Btn_Close)
	{
		Btn_Close->OnButtonClickedEvent.RemoveDynamic(this, &UPopup_HowToPlay::OnClickClose);
		Btn_Close->OnButtonClickedEvent.AddDynamic(this, &UPopup_HowToPlay::OnClickClose);
	}

	if (Btn_Prev)
	{
		Btn_Prev->OnButtonClickedEvent.RemoveDynamic(this, &UPopup_HowToPlay::OnClickPrev);
		Btn_Prev->OnButtonClickedEvent.AddDynamic(this, &UPopup_HowToPlay::OnClickPrev);
	}

	if (Btn_Next)
	{
		Btn_Next->OnButtonClickedEvent.RemoveDynamic(this, &UPopup_HowToPlay::OnClickNext);
		Btn_Next->OnButtonClickedEvent.AddDynamic(this, &UPopup_HowToPlay::OnClickNext);
	}

	PageTypes = InPageTypes;

	InitPageScrollView();

	UpdateNavigationButtons();
}

void UPopup_HowToPlay::InitPageScrollView()
{
	if (!PageScrollView)
		return;

	// 5개 페이지 생성
	PageScrollView->SetNumberOfPages(PageTypes.Num());

	// 페이지 변경 이벤트 바인딩
	PageScrollView->OnPageChanged.RemoveDynamic(this, &UPopup_HowToPlay::OnPageChanged);
	PageScrollView->OnPageChanged.AddDynamic(this, &UPopup_HowToPlay::OnPageChanged);

	// 각 페이지에 데이터 설정
	TArray<FHowToPlayPageData> PageDataArray = GetPageDataArray();
	for (int32 i = 0; i < PageDataArray.Num() && i < PageScrollView->GetTotalPages(); ++i)
	{
		if (auto Page = Cast<UPopup_HowToPlayItem>(PageScrollView->GetPageAt(i)))
		{
			Page->InitPageData(PageDataArray[i]);
		}
	}
}

void UPopup_HowToPlay::UpdateNavigationButtons()
{
	if (!PageScrollView)
		return;

	Btn_Prev->SetVisibility( PageTypes.Num() > 1 ? ESlateVisibility::Visible : ESlateVisibility::Hidden );
	Btn_Next->SetVisibility( PageTypes.Num() > 1 ? ESlateVisibility::Visible : ESlateVisibility::Hidden );
	
	int32 CurPage = PageScrollView->GetCurrentPage();
	int32 TotalPages = PageScrollView->GetTotalPages();

	// 이전 버튼 활성화/비활성화
	Btn_Prev->SetIsEnabled(CurPage > 0);

	// 다음 버튼 활성화/비활성화
	Btn_Next->SetIsEnabled(CurPage < TotalPages - 1);
}

void UPopup_HowToPlay::OnClickClose()
{
	// PopupManager를 통해 팝업 닫기
	if (UPopupManager* PopupMgr = UPopupManager::Get(GetWorld()))
	{
		PopupMgr->HideCurrentPopup();
	}

	// 델리게이트 호출
	if (OnClosedDelegate.IsBound())
	{
		OnClosedDelegate.Execute();
		OnClosedDelegate = nullptr;
	}
}

void UPopup_HowToPlay::OnClickPrev()
{
	PageScrollView->PrevPage();
}

void UPopup_HowToPlay::OnClickNext()
{
	PageScrollView->NextPage();
}

void UPopup_HowToPlay::OnPageChanged(int32 PrevPage, int32 CurrentPage)
{
	// 네비게이션 버튼 상태 업데이트
	UpdateNavigationButtons();
}

TArray<FHowToPlayPageData> UPopup_HowToPlay::GetPageDataArray() const
{
	TArray<FHowToPlayPageData> RetData;

	if ( auto DM = UGameDataManager::Get(GetWorld()) )
	{
		for (const EHowToPlayPageType& pageType : PageTypes)
		{
			FHowToPlayPageData pageData;
			if (DM->GetHowToPlayPageData(pageType, pageData))
				RetData.Add(pageData);
		}
	}

	return RetData;
}