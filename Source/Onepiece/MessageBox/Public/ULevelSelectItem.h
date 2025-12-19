// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ULevelSelectItem.generated.h"

DECLARE_DELEGATE_OneParam(FOnLevelSelectedDelegate, int32 /*Level*/);

/**
 * @brief 레벨 선택 아이템 위젯
 * @details 특정 레벨의 단어 그룹들(Animal, Color, Region, Food)을 표시하고 선택할 수 있는 위젯
 */
UCLASS()
class ONEPIECE_API ULevelSelectItem : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	virtual void NativeConstruct() override;
	
public:
	/**
	 * @brief 레벨 정보를 초기화하고 단어 그룹들을 생성
	 * @param InLevel 난이도 레벨 (1, 2, 3)
	 * @param InLevelName 레벨 이름 (Begginer, Intermediate, Advanced)
	 */
	UFUNCTION(BlueprintCallable, Category = "LevelSelectItem")
	void InitLevelItem(int32 InLevel, const FString& InLevelName, int32 InPlayTime);

protected:
	void CreateAnimalGroup();
	void CreateColorGroup();
	void CreateRegionGroup();
	void CreateFoodGroup();

	/**
	 * @brief 선택 버튼 클릭 이벤트
	 */
	UFUNCTION()
	void OnSelectButtonClicked();

public:
	/** 레벨 선택 델리게이트 */
	FOnLevelSelectedDelegate OnLevelSelected;

public:
	/** 레벨 타이틀 텍스트 (Begginer, Intermediate 등) */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UTextBlock> Txt_Title;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UTextBlock> Txt_PlayTime;

	
	/** 단어 그룹들이 배치될 Vertical Box */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UVerticalBox> VerticalBox;

	/** 선택 버튼 */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UImageButton> Btn_Select;

protected:
	/** 현재 레벨 */
	UPROPERTY()
	int32 Level = 1;

	/** 레벨 이름 */
	UPROPERTY()
	FString LevelName;

	/** 현재 레벨 */
	UPROPERTY()
	int32 PlayTime = 3;


	/** 생성할 WordGroup 위젯 클래스 */
	UPROPERTY(EditAnywhere, Category = "LevelSelectItem")
	TSubclassOf<class ULevelWordGroup> WordGroupClass;
};
