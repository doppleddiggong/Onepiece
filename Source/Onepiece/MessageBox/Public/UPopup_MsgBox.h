// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.

#pragma once

#include "CoreMinimal.h"
#include "UBasePopup.h"
#include "Blueprint/UserWidget.h"
#include "UPopup_MsgBox.generated.h"


UENUM(BlueprintType)
enum class EMsgBoxType : uint8
{
	OK,
	OK_CANCEL,
};

UCLASS()
class ONEPIECE_API UPopup_MsgBox : public UBasePopup
{
	GENERATED_BODY()

public:
	void InitPopup(
		const FString& InTitle,
		const FString& InDescription,
		EMsgBoxType InType,
		const FOnMsgBoxOkDelegate& InOkDelegate,
		const FOnMsgBoxCancelDelegate& InCancelDelegate);

	void InitPopup(
		const FString& InTitle,
		const FString& InDescription,
		EMsgBoxType InType,
		const FOnMsgBoxOkDelegate& InOkDelegate);

protected:
	virtual void NativeConstruct() override;

private:
	void SetTitle(const FString& InTitle);
	void SetDesc(const FString& InDescription);
	void InitButton(EMsgBoxType InType);
	
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
	class USpacer* Spacing_OkCancel;
	
	UPROPERTY(meta = (BindWidget))
	class UImageButton* Btn_Cancel;

private:
	FOnMsgBoxOkDelegate OnOkDelegate;
	FOnMsgBoxCancelDelegate OnCancelDelegate;
};
