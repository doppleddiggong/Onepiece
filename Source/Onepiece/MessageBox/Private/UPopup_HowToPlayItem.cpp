// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.

#include "UPopup_HowToPlayItem.h"
#include "UGameDataManager.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBoxSlot.h"

void UPopup_HowToPlayItem::InitPageData(const FHowToPlayPageData& InPageData)
{
	PageData = InPageData;

	Txt_Title->SetText(PageData.Title);

	Txt_Description->SetText(PageData.Description);

	Img_Icon->SetBrushFromTexture(PageData.Texture);
}