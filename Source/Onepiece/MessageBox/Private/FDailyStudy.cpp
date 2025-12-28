// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.

#include "FDailyStudy.h"

UTexture2D* FDailyStudyWordItem::LoadedTexture()
{
	if (Texture.IsValid())
		return Texture.Get();
	else if (!Texture.IsNull())
		return Texture.LoadSynchronous();

	return nullptr;
}