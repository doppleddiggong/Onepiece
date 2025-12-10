// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "WriteBoard.generated.h"

/**
 * 
 */
UCLASS()
class ONEPIECE_API UWriteBoard : public UObject
{
	GENERATED_BODY()
	
public:
	bool SaveRenderTargetToPNG(class UTextureRenderTarget2D* RenderTarget, const FString& FullFilePath);
	
	// TODO: SaveCanvas, ClearCanvas 둘 다 인자 어캐할 지 정하기. 지금 에러 안 나도록 이렇게 한 거
	void SaveCanvas(int32 Qid, UTextureRenderTarget2D* RT_Canvas);
	void ClearCanvas(UTextureRenderTarget2D* RT_Canvas);

private:
	const FString filePath = FPaths::ProjectSavedDir() / TEXT("WriteImage/");
};
