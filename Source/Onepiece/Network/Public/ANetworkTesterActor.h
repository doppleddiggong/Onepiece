// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.

/// @file ANetworkTesterActor.h
/// @brief 네트워크 진단용 테스트 액터를 선언합니다.
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "NetworkData.h"
#include "ANetworkTesterActor.generated.h"

UCLASS()
class ONEPIECE_API ANetworkTesterActor : public AActor
{
    GENERATED_BODY()

public:
    /// @brief 기본 컴포넌트를 구성하는 생성자입니다.
    ANetworkTesterActor();

public:
    // =============================================================================
    // User API Tests
    // =============================================================================

    /// @brief 테스트에 사용할 사용자 이름입니다.
    UPROPERTY(EditAnywhere, Category = "TEST|User")
    FString UserName = TEXT("test_user");

    /// @brief 사용자 등록 요청을 전송합니다 (POST /users/register).
    UFUNCTION(BlueprintCallable, CallInEditor, Category = "TEST|User")
    void RequestUserRegister();

    /// @brief OAuth2 토큰 발급 요청을 전송합니다 (POST /users/token).
    UFUNCTION(BlueprintCallable, CallInEditor, Category = "TEST|User")
    void RequestUserToken();

    /// @brief 현재 사용자 정보 조회 요청을 전송합니다 (GET /users/me).
    UFUNCTION(BlueprintCallable, CallInEditor, Category = "TEST|User")
    void RequestUserMe();
    
    // =============================================================================
    // OCR API Tests
    // =============================================================================

    /// @brief OCR 테스트용 이미지 파일 경로입니다 (프로젝트 루트 기준).
    UPROPERTY(EditAnywhere, Category = "TEST|OCR")
    FString OcrImagePath = TEXT("Sample/ocr_sample2.png");

    /// @brief OCR 텍스트 추출 요청을 전송합니다 (POST /writes/ocr/extract).
    UFUNCTION(BlueprintCallable, CallInEditor, Category = "TEST|OCR")
    void RequestOcrExtract();

    
    // =============================================================================
    // STT Voice Tests
    // =============================================================================

    /// @brief Speaking 테스트용 질문 입니다
    UPROPERTY(EditAnywhere, Category = "TEST|Voice")
    FString SpeakingQuestion = TEXT("Where are you from");
    
    /// @brief Speaking 테스트용 오디오 파일 경로입니다 (프로젝트 루트 기준).
    UPROPERTY(EditAnywhere, Category = "TEST|Voice")
    FString SpeakingAudioPath = TEXT("Sample/voice_sample.wav");
    
    /// @brief Speaking 답변 요청을 전송합니다 (POST /speakings/questions).
    UFUNCTION(BlueprintCallable, CallInEditor, Category = "TEST|Voice")
    void RequestSpeakingQuestions();

    // =============================================================================
    // TTS Voice Tests
    // =============================================================================
    UPROPERTY(EditAnywhere, Category = "TEST|Voice")
    FString AudioText  = TEXT("한국에서 가장 먼저 방문할 곳은 어디입니까?");
    
    UFUNCTION(BlueprintCallable, CallInEditor, Category = "TEST|Voice")
    void RequestListenAudio();

    
    // =============================================================================
    // Interview API
    // =============================================================================
    UFUNCTION(BlueprintCallable, CallInEditor, Category = "06.TEST|Interview")
    void RequestInterviewHello();

    
    
private:
    void OnResponseUserRegister(FResponseUserRegister& ResponseData, bool bWasSuccessful);
    void OnResponseUserToken(FResponseUserToken& ResponseData, bool bWasSuccessful);
    void OnResponseUserMe(FResponseUserMe& ResponseData, bool bWasSuccessful);
    
    void OnResponseOcrExtract(FResponseWriteSubmit& ResponseData, bool bWasSuccessful);

    void OnResponseSpeakingJudes(FResponseSpeakingJudes& ResponseData, bool bWasSuccessful);
    void OnResponseListenAudio(FResponseListenAudio& ResponseData, bool bWasSuccessful);

    void OnResponseInterviewHello(FResponseInterviewHello& ResponseData, bool bWasSuccessful);


private:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Voice", meta=(AllowPrivateAccess="true"))
    TObjectPtr<class UVoiceConversationSystem> VoiceConversationSystem;
};