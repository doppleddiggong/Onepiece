// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MessageBox.generated.h"

/**
 * 
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnUserNameRegister, const FString&, UserInput);

UCLASS()
class ONEPIECE_API UMessageBox : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;

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

	UPROPERTY(meta = (BindWidget))
	class UEditableTextBox* Edit_Name;

	UFUNCTION()
	void OnClickOk();
	
	UFUNCTION()
	void OnClickCancel();

public:
	void SetTitle(const FString& InTitle);
	void SetDesc(const FString& InDescription);

public:
	// Set Visibility
	// Name 입력 부분 표시
	void SetNameFieldVisibility(bool InVisibility);
	
public:
	// Deligates
	FOnUserNameRegister OnUserNameRegister;
};
