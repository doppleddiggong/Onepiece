// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UReadQuestWidget.generated.h"

/// @brief Read 퀘스트 메인 위젯
/// @details Step1(Read) 퀘스트의 UI를 관리합니다.
UCLASS()
class ONEPIECE_API UReadQuestWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UReadQuestWidget(const FObjectInitializer& ObjectInitializer);

protected:
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

public:
	//--------------------------------------------------------------//
	// BindWidget Components
	//--------------------------------------------------------------//

	/// @brief 문제1 선택 스크롤 박스
	UPROPERTY(meta = (BindWidget), BlueprintReadOnly)
	class UScrollBox* SymbolScrollBox;

	/// @brief 문제2 선택 스크롤 박스
	UPROPERTY(meta = (BindWidget), BlueprintReadOnly)
	class UScrollBox* ColorScrollBox;

	/// @brief 미션 설명 텍스트
	UPROPERTY(meta = (BindWidget), BlueprintReadOnly)
	class UTextBlock* MissionDescriptionText;

	/// @brief 타이머 텍스트
	UPROPERTY(meta = (BindWidget), BlueprintReadOnly)
	class UTextBlock* TimerText;

	//--------------------------------------------------------------//
	// Cached References
	//--------------------------------------------------------------//

protected:
	/// @brief GameState 참조 캐싱
	UPROPERTY()
	TObjectPtr<class ALingoGameState> CachedGameState;

	/// @brief PlayerState 참조 캐싱
	UPROPERTY()
	TObjectPtr<class ALingoPlayerState> CachedPlayerState;

	/// @brief 선택지 엔트리 위젯 클래스
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<class UReadQuestEntryWidget> EntryWidgetClass;

	/// @brief 현재 선택된 심볼 엔트리
	UPROPERTY()
	TObjectPtr<class UReadQuestEntryWidget> SelectedSymbolEntry;

	/// @brief 현재 선택된 색상 엔트리
	UPROPERTY()
	TObjectPtr<class UReadQuestEntryWidget> SelectedColorEntry;

	/// @brief 생성된 심볼 엔트리 목록
	UPROPERTY()
	TArray<TObjectPtr<class UReadQuestEntryWidget>> SymbolEntries;

	/// @brief 생성된 색상 엔트리 목록
	UPROPERTY()
	TArray<TObjectPtr<class UReadQuestEntryWidget>> ColorEntries;

	//--------------------------------------------------------------//
	// Methods
	//--------------------------------------------------------------//

public:
	/// @brief 퀘스트 위젯을 초기화합니다.
	UFUNCTION(BlueprintCallable, Category = "Quest")
	void InitializeQuest();

	/// @brief 심볼 리스트를 빌드합니다.
	UFUNCTION(BlueprintCallable, Category = "Quest")
	void BuildSymbolList();

	/// @brief 색상 리스트를 빌드합니다.
	UFUNCTION(BlueprintCallable, Category = "Quest")
	void BuildColorList();

protected:
	/// @brief 심볼 선택 처리
	/// @param Symbol [in] 선택된 심볼
	/// @param EntryWidget [in] 선택된 엔트리 위젯
	UFUNCTION()
	void OnSymbolSelected(const FString& Symbol, class UReadQuestEntryWidget* EntryWidget);

	/// @brief 색상 선택 처리
	/// @param Color [in] 선택된 색상
	/// @param EntryWidget [in] 선택된 엔트리 위젯
	UFUNCTION()
	void OnColorSelected(const FString& Color, class UReadQuestEntryWidget* EntryWidget);

	/// @brief 타이머 업데이트
	void UpdateTimer();

	/// @brief PlayerState 업데이트 처리
	void OnPlayerStateUpdated();

	/// @brief GameState와 PlayerState 참조를 캐싱합니다
	void CacheReferences();

	/// @brief 중복을 제거한 심볼 리스트를 반환합니다
	TArray<FString> GetUniqueSymbols() const;

	/// @brief 중복을 제거한 색상 리스트를 반환합니다
	TArray<FString> GetUniqueColors() const;
};
