// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.

#pragma once

#include "CoreMinimal.h"
#include "NetworkData.h"
#include "UBasePopup.h"
#include "Popup_QuestionnaireResultItem.generated.h"

/**
 * 
 */
UCLASS()
class ONEPIECE_API UPopup_QuestionnaireResultItem : public UBasePopup
{
	GENERATED_BODY()
	
public:		
	/** 질문 항목 초기화 */ 
	UFUNCTION(BlueprintCallable)
	void InitItem(int32 index, FString questionKor, const FResponseWriteData& data);

protected:
	/* ----------------- Layout ----------------- */
	/** "Question.01" 같은 질문 인덱스 */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UTextBlock> Text_Index;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UTextBlock> Text_Question;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class USizeBox> SizeBox_Answer;
	
	/** 플레이어가 작성한 답안 이미지 */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UImage> Image_Answer;
	
	/** "What is your current country..." 같은 질문 내용 */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UTextBlock> Text_Message;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UTextBlock> Text_Correction;

private:
	FResponseWriteData QuestionData;
	FString WriteImagePath = FPaths::ProjectSavedDir() / TEXT("WriteImage/");
	
	UTexture2D* LoadTextureFromFile(const FString& filePath);
};
