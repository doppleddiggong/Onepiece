// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MessageBox.generated.h"

/**
 * 
 */
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
	class UButton* Btn_X;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* Txt_Description;

	UPROPERTY(meta = (BindWidget))
	class UButton* Btn_Ok;

	UPROPERTY(meta = (BindWidget))
	class UButton* Btn_Cancel;

	UPROPERTY(meta = (BindWidget))
	class UEditableTextBox* Edt_Name;

	UFUNCTION()
	void OnOkPressed();
	
	UFUNCTION()
	void OnCancelPressed();

public:
	// Set Text
	void SetTitle(FString InTitle);
	void SetDescription(FString InDescription);
};
