// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.

#pragma once

#include "CoreMinimal.h"
#include "UBasePopup.h"
#include "UPopup_LevelSelect.generated.h"

/** 팝업 닫힘 델리게이트 */
DECLARE_DELEGATE(FOnPopupClosedDelegate);

/**
 * @brief 레벨 선택 팝업 위젯
 * @details 여러 난이도 레벨을 선택할 수 있는 팝업. 3개의 레벨 아이템을 표시하고 좌우 버튼으로 페이지 이동
 */
UCLASS()
class ONEPIECE_API UPopup_LevelSelect : public UBasePopup
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;

public:
	/**
	 * @brief 팝업을 초기화하고 레벨 아이템들을 생성
	 */
	UFUNCTION(BlueprintCallable, Category = "PopupLevelSelect")
	void InitPopup();

protected:
	/**
	 * @brief 레벨 선택 이벤트 핸들러
	 * @param SelectedLevel 선택된 레벨
	 */
	void OnLevelSelected(int32 SelectedLevel);

	UFUNCTION()
	void OnClickClose();

public:
	/** 팝업이 닫힐 때 호출되는 델리게이트 */
	FOnPopupClosedDelegate OnPopupClosed;

protected:
	/** 레벨 아이템들이 배치될 Horizontal Box */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UHorizontalBox> HorizontalBox;

	/** 팝업 닫기 버튼 (우측 상단) */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UTextureButton> Btn_Close;
	
	/** 생성할 LevelSelectItem 위젯 클래스 */
	UPROPERTY(EditAnywhere, Category = "PopupLevelSelect")
	TSubclassOf<class ULevelSelectItem> LevelSelectItemClass;

	/** 전체 레벨 개수 */
	UPROPERTY(EditAnywhere, Category = "PopupLevelSelect")
	int32 TotalLevels = 3;

	/** 레벨 이름 배열 */
	UPROPERTY(EditAnywhere, Category = "PopupLevelSelect")
	TArray<FString> LevelNames = {TEXT("Begginer"), TEXT("Intermediate"), TEXT("Advanced")};
};
