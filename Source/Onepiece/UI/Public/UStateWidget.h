// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UStateWidget.generated.h"

UCLASS(BlueprintType, Blueprintable)
class ONEPIECE_API UStateWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

    void InitWidget();

    void UpdateUserName(int32 InUserId, const FString& InUserName);
    
private:
    void UpdateSpectrumVisual(float DeltaTime);   

    UFUNCTION(BlueprintCallable, Category = "State|AudioCapture")
    void OnAudioCapture(bool bRecording);

    UFUNCTION(BlueprintCallable, Category = "State|AudioCapture")
    void OnAudioSpectrum(float Spectrum);
    
protected:
    UPROPERTY(meta = (BindWidget), BlueprintReadOnly, Category = "State|NickName")
    TObjectPtr<class UBorder> Border_State;

    UPROPERTY(meta = (BindWidget), BlueprintReadOnly, Category = "State|NickName")
    TObjectPtr<class UTextBlock> Txt_UserId;

    UPROPERTY(meta = (BindWidget), BlueprintReadOnly, Category = "State|NickName")
    TObjectPtr<class UTextBlock> Txt_UserName;
    
    UPROPERTY(meta = (BindWidgetOptional), BlueprintReadOnly, Category = "State|Audio")
    TObjectPtr<class UProgressBar> SpectrumProgressBar;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "State|Audio", meta = (ClampMin = "0.0"))
    float SpectrumDecaySpeed = 6.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "State|Audio", meta = (ClampMin = "0.0"))
    float SpectrumRiseSpeed = 12.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "State|Audio", meta = (ClampMin = "0.001", ClampMax = "1.0"))
    float SpectrumSmoothing = 0.2f;

private:
    float SpectrumDisplayValue = 0.f;
};
