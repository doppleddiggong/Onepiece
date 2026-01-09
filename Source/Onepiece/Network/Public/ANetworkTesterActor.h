// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.

/**
 * @file ANetworkTesterActor.h
 * @brief 네트워크 진단용 테스트 액터를 선언합니다.
 */
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "NetworkData.h"
#include "ANetworkTesterActor.generated.h"

/**
 * @brief KLingo 네트워크 API를 테스트하기 위한 진단 액터입니다.
 *
 * 에디터 또는 개발 환경에서 API 요청 흐름을 빠르게 검증할 수 있도록 제공됩니다.
 * 실제 게임플레이 로직과 분리된 테스트 전용 엔트리로, 요청 결과는 로그/UI에 표시됩니다.
 * 사용 위치: 개발/QA 단계에서 수동 API 검증 시.
 */
UCLASS()
class ONEPIECE_API ANetworkTesterActor : public AActor
{
    GENERATED_BODY()

public:
    /**
     * @brief 기본 컴포넌트를 구성하는 생성자입니다.
     *
     * 네트워크 테스트에 필요한 보조 컴포넌트 및 기본값을 초기화합니다.
     * 사이드이펙트: 테스트 환경에 필요한 객체 참조를 준비합니다.
     */
    ANetworkTesterActor();

public:
    // =============================================================================
    // User API Tests
    // =============================================================================

    /**
     * @brief 테스트에 사용할 사용자 이름입니다.
     *
     * 회원 등록/토큰 요청의 입력 값으로 사용됩니다.
     * 소유권/수명: 이 액터 인스턴스가 소유하며 에디터에서 수정됩니다.
     * 동기화: 네트워크 복제와 무관합니다.
     */
    UPROPERTY(EditAnywhere, Category = "TEST|User")
    FString UserName = TEXT("test_user");

    /**
     * @brief 사용자 등록 요청을 전송합니다 (POST /users/register).
     *
     * 테스트 계정 생성을 위한 API 호출을 수행합니다.
     * 사이드이펙트: 네트워크 요청을 전송하고 응답 로그를 출력합니다.
     */
    UFUNCTION(BlueprintCallable, CallInEditor, Category = "TEST|User")
    void RequestUserRegister();

    /**
     * @brief OAuth2 토큰 발급 요청을 전송합니다 (POST /users/token).
     *
     * 등록된 계정에 대해 인증 토큰을 발급받는 테스트 호출입니다.
     * 사이드이펙트: 네트워크 요청을 전송하고 응답 로그를 출력합니다.
     */
    UFUNCTION(BlueprintCallable, CallInEditor, Category = "TEST|User")
    void RequestUserToken();

    /**
     * @brief 현재 사용자 정보 조회 요청을 전송합니다 (GET /users/me).
     *
     * 토큰 기반 인증이 유효한지 확인하기 위한 테스트 호출입니다.
     * 사이드이펙트: 네트워크 요청을 전송하고 응답 로그를 출력합니다.
     */
    UFUNCTION(BlueprintCallable, CallInEditor, Category = "TEST|User")
    void RequestUserMe();
    
    // =============================================================================
    // OCR API Tests
    // =============================================================================

    /**
     * @brief OCR 테스트용 이미지 파일 경로입니다 (프로젝트 루트 기준).
     *
     * 이미지 파일을 읽어 서버로 전송하며, 문자열 OCR 결과 검증에 사용합니다.
     * 소유권/수명: 이 액터 인스턴스가 소유하며 에디터에서 수정됩니다.
     * 동기화: 네트워크 복제와 무관합니다.
     */
    UPROPERTY(EditAnywhere, Category = "TEST|OCR")
    FString OcrImagePath = TEXT("Sample/ocr_sample2.png");

    /**
     * @brief OCR 텍스트 추출 요청을 전송합니다 (POST /writes/ocr/extract).
     *
     * 지정된 이미지 파일을 업로드해 OCR 결과를 확인합니다.
     * 사이드이펙트: 네트워크 요청을 전송하고 응답 로그를 출력합니다.
     */
    UFUNCTION(BlueprintCallable, CallInEditor, Category = "TEST|OCR")
    void RequestOcrExtract();

    
    // =============================================================================
    // STT Voice Tests
    // =============================================================================

    /**
     * @brief Speaking 테스트용 질문 텍스트입니다.
     *
     * 음성 답변 평가 요청의 질문으로 사용되며 서버 기준 문항과 맞추어야 합니다.
     * 소유권/수명: 이 액터 인스턴스가 소유하며 에디터에서 수정됩니다.
     * 동기화: 네트워크 복제와 무관합니다.
     */
    UPROPERTY(EditAnywhere, Category = "TEST|Voice")
    FString SpeakingQuestion = TEXT("Where are you from");
    
    /**
     * @brief Speaking 테스트용 오디오 파일 경로입니다 (프로젝트 루트 기준).
     *
     * 음성 평가 요청의 입력 파일이며 WAV 등 서버가 지원하는 포맷이어야 합니다.
     * 소유권/수명: 이 액터 인스턴스가 소유하며 에디터에서 수정됩니다.
     * 동기화: 네트워크 복제와 무관합니다.
     */
    UPROPERTY(EditAnywhere, Category = "TEST|Voice")
    FString SpeakingAudioPath = TEXT("Sample/voice_sample.wav");
    
    /**
     * @brief Speaking 답변 요청을 전송합니다 (POST /speakings/questions).
     *
     * 질문 텍스트와 오디오 파일을 서버에 전송하여 발음 평가를 요청합니다.
     * 사이드이펙트: 네트워크 요청을 전송하고 응답 로그를 출력합니다.
     */
    UFUNCTION(BlueprintCallable, CallInEditor, Category = "TEST|Voice")
    void RequestSpeakingQuestions();

    // =============================================================================
    // TTS Voice Tests
    // =============================================================================
    /**
     * @brief TTS 테스트용 입력 텍스트입니다.
     *
     * 서버에서 음성 파일을 생성할 문장을 지정합니다.
     * 소유권/수명: 이 액터 인스턴스가 소유하며 에디터에서 수정됩니다.
     * 동기화: 네트워크 복제와 무관합니다.
     */
    UPROPERTY(EditAnywhere, Category = "TEST|Voice")
    FString AudioText  = TEXT("한국에서 가장 먼저 방문할 곳은 어디입니까?");
    
    /**
     * @brief Listen 음성 생성 요청을 전송합니다.
     *
     * 텍스트 기반 음성 생성 API 호출 결과를 검증합니다.
     * 사이드이펙트: 네트워크 요청을 전송하고 응답 로그를 출력합니다.
     */
    UFUNCTION(BlueprintCallable, CallInEditor, Category = "TEST|Voice")
    void RequestListenAudio();

    
    // =============================================================================
    // Interview API
    // =============================================================================
    /**
     * @brief 인터뷰 시작 요청을 전송합니다.
     *
     * 인터뷰 시나리오 제공 여부를 확인하는 테스트 호출입니다.
     * 사이드이펙트: 네트워크 요청을 전송하고 응답 로그를 출력합니다.
     */
    UFUNCTION(BlueprintCallable, CallInEditor, Category = "06.TEST|Interview")
    void RequestInterviewHello();

    // =============================================================================
    // Chat Answers API
    // =============================================================================

    /**
     * @brief Chat 답변 요청 테스트용 컨텍스트입니다.
     *
     * 대화 맥락을 지정해 응답 품질을 확인합니다.
     * 소유권/수명: 이 액터 인스턴스가 소유하며 에디터에서 수정됩니다.
     * 동기화: 네트워크 복제와 무관합니다.
     */
    UPROPERTY(EditAnywhere, Category = "07.TEST|Chat")
    FString ChatContext = TEXT("You are a helpful assistant.");

    /**
     * @brief Chat 답변 요청 테스트용 질문입니다.
     *
     * 텍스트 질문으로 답변 API를 테스트합니다.
     * 소유권/수명: 이 액터 인스턴스가 소유하며 에디터에서 수정됩니다.
     * 동기화: 네트워크 복제와 무관합니다.
     */
    UPROPERTY(EditAnywhere, Category = "07.TEST|Chat")
    FString ChatQuestion = TEXT("살려주세요");

    /**
     * @brief Chat 답변 요청 테스트용 오디오 파일 경로입니다.
     *
     * 음성 질문 기반 답변 API를 테스트할 때 사용합니다.
     * 소유권/수명: 이 액터 인스턴스가 소유하며 에디터에서 수정됩니다.
     * 동기화: 네트워크 복제와 무관합니다.
     */
    UPROPERTY(EditAnywhere, Category = "07.TEST|Chat")
    FString ChatAudioPath = TEXT("Sample/voice_sample.wav");

    /**
     * @brief Chat 답변 요청을 전송합니다 (텍스트 질문). POST /chats/answers
     *
     * 지정된 컨텍스트와 질문으로 챗봇 답변을 요청합니다.
     * 사이드이펙트: 네트워크 요청을 전송하고 응답 로그를 출력합니다.
     */
    UFUNCTION(BlueprintCallable, CallInEditor, Category = "07.TEST|Chat")
    void RequestChatAnswers();

    /**
     * @brief Chat 답변 요청을 전송합니다 (음성 질문). POST /chats/answers
     *
     * 음성 파일을 업로드해 답변을 요청합니다.
     * 사이드이펙트: 네트워크 요청을 전송하고 응답 로그를 출력합니다.
     */
    UFUNCTION(BlueprintCallable, CallInEditor, Category = "07.TEST|Chat")
    void RequestChatAnswersWithAudio();



private:
    void OnResponseUserRegister(FResponseUserRegister& ResponseData, bool bWasSuccessful);
    void OnResponseUserToken(FResponseUserToken& ResponseData, bool bWasSuccessful);
    void OnResponseUserMe(FResponseUserMe& ResponseData, bool bWasSuccessful);
    
    void OnResponseOcrExtract(FResponseWriteSubmit& ResponseData, bool bWasSuccessful);

    void OnResponseSpeakingJudes(FResponseSpeakingJudes& ResponseData, bool bWasSuccessful);
    void OnResponseListenAudio(FResponseListenAudio& ResponseData, bool bWasSuccessful);

    void OnResponseInterviewHello(FResponseInterviewHello& ResponseData, bool bWasSuccessful);

    void OnResponseChatAnswers(FResponseChatAnswers& ResponseData, bool bWasSuccessful);


private:
    /**
     * @brief 음성 대화 시스템 참조입니다.
     *
     * 네트워크 테스트 과정에서 음성 처리 파이프라인을 연동하기 위해 사용합니다.
     * 소유권/수명: 이 액터가 소유하며 런타임 동안 유지됩니다.
     * 동기화: 네트워크 복제와 무관합니다.
     */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Voice", meta=(AllowPrivateAccess="true"))
    TObjectPtr<class UVoiceConversationSystem> VoiceConversationSystem;
};
