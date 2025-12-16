// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.

#pragma once

#include "CoreMinimal.h"
#include "NetworkData.h"
#include "UBasePopup.h"
#include "Popup_QuestionnaireResult.generated.h"

/**
 * 
 */
UCLASS()
class ONEPIECE_API UPopup_QuestionnaireResult : public UBasePopup
{
	GENERATED_BODY()
	
public:
	UPopup_QuestionnaireResult(const FObjectInitializer& ObjectInitializer);
	
	/** 팝업 초기화 (필요 시 API 확장) */
	UFUNCTION(BlueprintCallable)
	void InitPopup(const FResponseWriteSubmit& InResponseData, const FResponseWriteResult& InResponseResultData);

protected:
	virtual void NativeConstruct() override;

private:
	void InitWholeResult(const FResponseWriteResult& ResponseData);
	
	UFUNCTION()
	void OnClickClose();

protected:
	/** 팝업 전체 배경 */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UImage> Popup_BaseBg;

	/** 인터뷰 항목들이 추가될 VerticalBox */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UScrollBox> ScrollBox_Result;
	
	/** 전체 결과 */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UResultStatWidget> Result_AverageScore;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UResultStatWidget> Result_Time;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UResultStatWidget> Result_Grade;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UResultStatWidget> Result_TopRate;

	/** 팝업 내에서 표시될 인터뷰 항목 위젯 클래스 */
	UPROPERTY(EditDefaultsOnly, Category="Popup")
	TSubclassOf<class UPopup_QuestionnaireResultItem> QuestionnaireResultItemClass;

	/** 인터뷰 항목 간 간격 (Spacer Height) */
	UPROPERTY(EditDefaultsOnly, Category="Popup", meta=(ClampMin="0.0", ClampMax="200.0"))
	float ItemSpacing = 20.0f;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UImageButton> Btn_Confirm;

private:
	FResponseWriteSubmit ResponseData;
	TArray<FString> QuestionsKor;
	TArray<FString> QuestionsEng;
	
	FString WriteImagePath = FPaths::ProjectSavedDir() / TEXT("WriteImage/");
};
