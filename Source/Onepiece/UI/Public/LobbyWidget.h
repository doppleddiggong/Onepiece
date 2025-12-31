// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "LobbyWidget.generated.h"

/**
 * 
 */
UCLASS()
class ONEPIECE_API ULobbyWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;

public:
	UPROPERTY()
	class ULingoGameInstance* GI;
	
	UPROPERTY(meta=(BindWidget))
	class UWidgetSwitcher* widgetSwitcher;
	
public:
	// 0 : Lobby Canvas
	UPROPERTY(meta = (BindWidget))
	class UHoverButton* Btn_Host;

	UPROPERTY(meta = (BindWidget))
	class UHoverButton* Btn_Join;

	UPROPERTY(meta = (BindWidget))
	class UHoverButton* Btn_LogOut;

	UFUNCTION()
	void OnHostClicked();
	
	UFUNCTION()
	void OnJoinClicked();

	UFUNCTION()
	void OnLogOutClicked();

public:
	// 1 : Create Canvas
	UPROPERTY(meta = (BindWidget))
	class UEditableTextBox* EdtTxt_SessionName;

	UPROPERTY(meta = (BindWidget))
	class UImageButton* Btn_Create;

	UPROPERTY(meta = (BindWidget))
	class UHoverButton* Btn_Back1;

	UFUNCTION()
	void OnCreateClicked();
	// EdtTxt_SessionName의 내용 변경 시 호출
	UFUNCTION()
	void OnValueChangedSessionName(const FText& Text);

	UFUNCTION()
	void OnBackPressed();

public:
	// 2 : Join Canvas
	UPROPERTY(meta = (BindWidget))
	class UScrollBox* Scrl_SessionList;

	UPROPERTY(meta = (BindWidget))
	class UTextureButton* Btn_Update;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* Txt_Update;

	UPROPERTY(meta = (BindWidget))
	class UHoverButton* Btn_Back2;
	
	// 세션 위젯
	UPROPERTY(EditAnywhere)
	TSubclassOf<class USessionInfoWidget> sessionInfoWidget;

	// 조회 버튼 클릭시 호출되는 함수
	UFUNCTION()
	void OnClickFind();
	// 세션 정보를 받아 SessionInfoWidget 을 만드는 함수
	// (NetGameInstance 의 onFindComplete 딜리게이트에 등록할 함수)
	UFUNCTION()
	void OnFindComplete(int32 idx, FString sessionName);	
};
