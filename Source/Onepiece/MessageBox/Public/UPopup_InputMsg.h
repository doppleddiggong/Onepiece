// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.

#pragma once

#include "CoreMinimal.h"
#include "NetworkData.h"
#include "UBasePopup.h"
#include "UPopupManager.h"
#include "Blueprint/UserWidget.h"
#include "UPopup_InputMsg.generated.h"

UCLASS()
class ONEPIECE_API UPopup_InputMsg : public UBasePopup
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;

private:
	void SetTitle(const FString& InTitle);
	void SetDesc(const FString& InDescription);
	
	UFUNCTION()
	void OnClickOk();
	
	UFUNCTION()
	void OnClickCancel();

public:
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* Txt_Title;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* Txt_Desc;

	UPROPERTY(meta = (BindWidget))
	class UTextureButton* Btn_Close;

	UPROPERTY(meta = (BindWidget))
	class UImageButton* Btn_Ok;

	UPROPERTY(meta = (BindWidget))
	class UImageButton* Btn_Cancel;

public:
	void InitPopup( const EPopupType PopupType );

private:
	void RequestUserRegister(const FString& UserInput);
	void OnResponseUserRegister(FResponseUserRegister& ResponseData, bool bWasSuccessful);

	void RequestUserToken(const FString& UserInput);
	void OnResponseUserToken(FResponseUserToken& ResponseData, bool bWasSuccessful);

public:
	UPROPERTY(meta = (BindWidget))
	class UEditableTextBox* Edit_Name;
};
