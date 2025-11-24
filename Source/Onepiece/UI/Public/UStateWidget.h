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
    virtual void NativeConstruct() override;
    virtual void NativeDestruct() override;
    virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

    void InitWidget();
    
private:
    void UpdateSpectrumVisual(float DeltaTime);
    
    void UpdateLoadingSpinner(float DeltaTime);

    UFUNCTION(BlueprintCallable, Category = "State|Network")
    void OnNetworkWaitCount(int NetworkWaitCount);

    UFUNCTION(BlueprintCallable, Category = "State|AudioCapture")
    void OnAudioCapture(bool bRecording);

    UFUNCTION(BlueprintCallable, Category = "State|AudioCapture")
    void OnAudioSpectrum(float Spectrum);

    UFUNCTION(BlueprintCallable, Category = "State|UserName")
    void OnUpdateUserName(FString UserName);

    
protected:
    UPROPERTY(meta = (BindWidget), BlueprintReadOnly, Category = "State|NickName")
    class UTextBlock* UserNameText = nullptr;

    UPROPERTY(meta = (BindWidgetOptional), BlueprintReadOnly, Category = "State|Network")
    class UImage* LoadingSpinner = nullptr;

    UPROPERTY(meta = (BindWidgetOptional), BlueprintReadOnly, Category = "State|Audio")
    class UProgressBar* SpectrumProgressBar = nullptr;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "State|Network")
    float SpinnerRotationSpeed = 90.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "State|Audio", meta = (ClampMin = "0.0"))
    float SpectrumDecaySpeed = 6.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "State|Audio", meta = (ClampMin = "0.0"))
    float SpectrumRiseSpeed = 12.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "State|Audio", meta = (ClampMin = "0.001", ClampMax = "1.0"))
    float SpectrumSmoothing = 0.2f;

private:
    FTimerHandle UpdateTimerHandle;
    float SpectrumDisplayValue = 0.f;
};
