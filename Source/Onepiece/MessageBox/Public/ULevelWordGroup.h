// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.

#pragma once

#include "CoreMinimal.h"
#include "EWordType.h"
#include "Blueprint/UserWidget.h"
#include "ULevelWordGroup.generated.h"

/**
 * @brief 특정 WordType의 단어 아이템들을 그룹으로 묶어서 표시하는 위젯
 * @details Uniform Grid Panel에 ULevelWordItem들을 배치하여 단어 그룹을 시각화
 */
UCLASS()
class ONEPIECE_API ULevelWordGroup : public UUserWidget
{
	GENERATED_BODY()

public:
	/**
	 * @brief 그룹 정보를 초기화하고 아이템들을 생성
	 * @param InWordType 단어 타입 (Animal, Color, Region, Food)
	 * @param InCurrentLevel 현재 선택된 레벨 (이 레벨 이하의 모든 단어를 표시)
	 */
	void InitGroup(EWordType InWordType, int32 InCurrentLevel);

	/**
	 * @brief 그리드에 표시할 열의 개수를 설정
	 * @param InColumnCount 열 개수
	 */
	void SetColumnCount(int32 InColumnCount);

protected:
	/**
	 * @brief 그룹 타이틀을 업데이트
	 */
	void UpdateGroupTitle();

	/**
	 * @brief 아이템들을 Uniform Grid Panel에 배치
	 */
	void PopulateItems();

public:
	/** 그룹 심볼 이미지 */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UImage> Image_Symbol;

	/** 그룹 이름 텍스트 */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UTextBlock> Text_GroupName;

	/** 아이템들이 배치될 Uniform Grid Panel */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UUniformGridPanel> UniformGridPanel;

protected:
	/** 현재 그룹의 단어 타입 */
	UPROPERTY()
	EWordType WordType;

	/** 현재 선택된 레벨 */
	UPROPERTY()
	int32 CurrentLevel = 1;

	/** 그리드 열 개수 (기본값: 2) */
	UPROPERTY(EditAnywhere, Category = "LevelWordGroup")
	int32 ColumnCount = 2;

	/** 생성할 아이템 위젯 클래스 */
	UPROPERTY(EditAnywhere, Category = "LevelWordGroup")
	TSubclassOf<class ULevelWordItem> LevelWordItemClass;
};
