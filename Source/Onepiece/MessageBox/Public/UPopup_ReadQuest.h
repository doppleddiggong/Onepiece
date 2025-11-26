// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.

#pragma once

#include "CoreMinimal.h"
#include "NetworkData.h"
#include "Blueprint/UserWidget.h"
#include "UPopup_ReadQuest.generated.h"

/// @brief Read 퀘스트 메인 위젯
/// @details Step1(Read) 퀘스트의 UI를 관리합니다.
UCLASS()
class ONEPIECE_API UPopup_ReadQuest : public UUserWidget
{
	GENERATED_BODY()

public:
	UPopup_ReadQuest(const FObjectInitializer& ObjectInitializer);

protected:
	virtual void NativeConstruct() override;

public:
	/// @brief 퀘스트 위젯을 초기화합니다.
	UFUNCTION(BlueprintCallable, Category = "Quest")
	void InitPopup(const FResponseScenario& InScenarioData);

protected:
	/// @brief 심볼 선택 처리
	/// @param Symbol [in] 선택된 심볼
	/// @param EntryWidget [in] 선택된 엔트리 위젯
	UFUNCTION()
	void OnSelectWord1(const FString& Symbol, class UPopup_ReadQuestItem* EntryWidget);

	/// @brief 색상 선택 처리
	/// @param Color [in] 선택된 색상
	/// @param EntryWidget [in] 선택된 엔트리 위젯
	UFUNCTION()
	void OnSelectWord2(const FString& Color, class UPopup_ReadQuestItem* EntryWidget);

	/// @brief PlayerState 업데이트 처리
	void OnPlayerStateUpdated();

private:
	// /// @brief 심볼 리스트를 빌드합니다.
	// UFUNCTION(BlueprintCallable, Category = "Quest")
	// void InitWord1List();
	//
	// /// @brief 색상 리스트를 빌드합니다.
	// UFUNCTION(BlueprintCallable, Category = "Quest")
	// void InitWord2List();

	UFUNCTION(BlueprintCallable, Category = "Close")
	void OnClickClose();
public:
	//--------------------------------------------------------------//
	// BindWidget Components
	//--------------------------------------------------------------//

	/// @brief 미션 설명 텍스트
	UPROPERTY(meta = (BindWidget), BlueprintReadOnly)
	class UWordWidget* WordWidget;
	
	// /// @brief 문제1 선택 스크롤 박스
	// UPROPERTY(meta = (BindWidget), BlueprintReadOnly)
	// class UScrollBox* Scroll_Word1;
	//
	// /// @brief 문제2 선택 스크롤 박스
	// UPROPERTY(meta = (BindWidget), BlueprintReadOnly)
	// class UScrollBox* Scroll_Word2;

	/// @brief 미션 설명 텍스트
	UPROPERTY(meta = (BindWidget), BlueprintReadOnly)
	class UTextBlock* Txt_Desc;

	/// @brief 미션 설명 텍스트
	UPROPERTY(meta = (BindWidget), BlueprintReadOnly)
	class UButton* Btn_Close;
// 	
// protected:
// 	UPROPERTY(EditDefaultsOnly, Category = "UI")
// 	TSubclassOf<class UPopup_ReadQuestItem> EntryWidgetClass;
//
// 	/// @brief 현재 선택된 심볼 엔트리
// 	UPROPERTY()
// 	TObjectPtr<class UPopup_ReadQuestItem> SelectedSymbolEntry;
//
// 	/// @brief 현재 선택된 색상 엔트리
// 	UPROPERTY()
// 	TObjectPtr<class UPopup_ReadQuestItem> SelectedColorEntry;
//
// 	/// @brief 생성된 심볼 엔트리 목록
// 	UPROPERTY()
// 	TArray<TObjectPtr<class UPopup_ReadQuestItem>> Word1EntryList;
//
// 	/// @brief 생성된 색상 엔트리 목록
// 	UPROPERTY()
// 	TArray<TObjectPtr<class UPopup_ReadQuestItem>> Word2EntryList;
	
private:
	FResponseScenario ScenarioData;
};
