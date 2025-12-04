// Copyright (c) 2025 Doppleddiggong. All rights reserved.
// Unauthorized copying, modification, or distribution of this file,
// via any medium is strictly prohibited. Proprietary and confidential.

/// @file UKLingoNetworkSystem.h
/// @brief KLingo API 요청을 담당하는 서브시스템을 선언합니다.
#pragma once

#include "CoreMinimal.h"
#include "NetworkData.h"
#include "Macro.h"
#include "ENetworkLogType.h"
#include "Interfaces/IHttpRequest.h"
#include "Interfaces/IHttpResponse.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "UKLingoNetworkSystem.generated.h"

UCLASS()
class ONEPIECE_API UKLingoNetworkSystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	DEFINE_SUBSYSTEM_GETTER_INLINE(UKLingoNetworkSystem);

	/// @brief HTTP 모듈 초기화 및 네트워크 대기 카운터를 설정합니다.
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	/// @brief 등록된 델리게이트를 정리하고 서브시스템을 종료합니다.
	virtual void Deinitialize() override;

	void RequestUserRegister(const FString& UserName, FResponseUserRegisterDelegate InDelegate);
	void RequestUserToken(const FString& UserName, FResponseUserTokenDelegate InDelegate);
	void RequestUserMe(FResponseUserMeDelegate InDelegate);
	void RequestUserHost(FResponseUserHostDelegate InDelegate);
	
	/// @brief Scenario 데이터를 조회합니다.
	/// @param Index [in] 시나리오 인덱스입니다.
	/// @param Difficulty [in] 난이도입니다.
	/// @param Level [in] 레벨 설정입니다 (1: 한국어).
	/// @param InDelegate [in] 응답 수신 시 호출될 델리게이트입니다.
	void RequestScenario(int32 Index, int32 Difficulty, int32 Level, FResponseScenarioDelegate InDelegate);

	/// @brief 이미지에서 OCR로 텍스트를 추출합니다.
	/// @param ImagePath [in] 이미지 파일 경로입니다.
	/// @param InDelegate [in] 응답 수신 시 호출될 델리게이트입니다.
	void RequestOcrExtract(const TArray<FString>& ImagePathArray, FString InTargetText, FResponseOcrExtractDelegate InDelegate);

	void RequestListenAudio(const FString& AudioText, FResponseListenAudioDelegate InDelegate);
	/// @brief 음성 파일을 전송하고 답변을 받습니다.
	/// @param  Question [in] 질문 받은 내용
	/// @param AudioPath [in] WAV 오디오 파일 경로입니다.
	/// @param InDelegate [in] 응답 수신 시 호출될 델리게이트입니다.
	void RequestSpeakingJudges( const FString& Question, const FString& AudioPath, FResponseSpeakingJudesDelegate InDelegate);

	void RequestInterviewHello(FResponseInterviewHelloDelegate InDelegate);
	void RequestInterviewAnswer( const FRequestInterviewAnswer& Answer, FResponseInterviewAnswerDelegate InDelegate);
	void RequestQuestResult(const FRequestReadQuestResult& Result, FResponseQuestResultDelegate InDelegate);


	

	/*
	/// @brief 계정으로 로그인 요청을 전송합니다.
	/// @param Account [in] 사용자 계정 ID입니다.
	/// @param InDelegate [in] 응답 수신 시 호출될 델리게이트입니다.
	void RequestLogin(const FString& Account, FResponseLoginDelegate InDelegate);

	/// @brief 새 사용자 정보를 생성합니다.
	/// @param Character [in] 캐릭터 인덱스입니다.
	/// @param CharacterColor [in] 캐릭터 색상 인덱스입니다.
	/// @param InDelegate [in] 응답 수신 시 호출될 델리게이트입니다.
	void RequestCreateUser(int32 Character, int32 CharacterColor, FResponseCreateUserDelegate InDelegate);

	/// @brief 인터뷰 요청을 전송합니다.
	/// @param InDelegate [in] 응답 수신 시 호출될 델리게이트입니다.
	void RequestInterview(FResponseInterviewDelegate InDelegate);

	/// @brief 인터뷰 답변을 제출합니다.
	/// @param Answers [in] 인터뷰 답변 배열입니다.
	/// @param InDelegate [in] 응답 수신 시 호출될 델리게이트입니다.
	void RequestSubmitInterview(const TArray<FString>& Answers, FResponseInterviewDelegate InDelegate);

	/// @brief 게임 시작 요청을 전송합니다.
	/// @param PlayerList [in] 플레이어 ID 목록입니다.
	/// @param Nicknames [in] 플레이어 닉네임 목록입니다.
	/// @param InDelegate [in] 응답 수신 시 호출될 델리게이트입니다.
	void RequestStartGame(const TArray<FString>& PlayerList, const TArray<FString>& Nicknames, FResponseStartGameDelegate InDelegate);

	/// @brief 게임 로그인 요청을 전송합니다.
	/// @param InDelegate [in] 응답 수신 시 호출될 델리게이트입니다.
	void RequestGameLogin(FResponseGameLoginDelegate InDelegate);

	/// @brief 퀘스트 답변을 제출합니다.
	/// @param QuestStep [in] 퀘스트 단계입니다.
	/// @param QuestAnswer [in] 퀘스트 답변입니다.
	/// @param PlayTime [in] 플레이 시간입니다.
	/// @param InDelegate [in] 응답 수신 시 호출될 델리게이트입니다.
	void RequestQuestAnswer(int32 QuestStep, int32 QuestAnswer, float PlayTime, FResponseQuestAnswerDelegate InDelegate);

	/// @brief 퀘스트 실패를 보고합니다.
	/// @param QuestStep [in] 실패한 퀘스트 단계입니다.
	/// @param InDelegate [in] 응답 수신 시 호출될 델리게이트입니다.
	void RequestQuestFail(int32 QuestStep, FResponseQuestAnswerDelegate InDelegate);

	/// @brief Write 퀘스트 답변을 제출합니다 (Multipart Form Data 사용).
	/// @param ImagePaths [in] 이미지 파일 경로 배열입니다.
	/// @param TextData [in] 텍스트 데이터 배열입니다.
	/// @param InDelegate [in] 응답 수신 시 호출될 델리게이트입니다.
	void RequestQuestWrite(const TArray<FString>& ImagePaths, const TArray<FString>& TextData, FResponseQuestWriteDelegate InDelegate);

	/// @brief Speak 퀘스트 답변을 제출합니다 (Multipart Form Data 사용).
	/// @param SpeakStep [in] Speak 단계입니다.
	/// @param WavFilePath [in] WAV 오디오 파일 경로입니다.
	/// @param InDelegate [in] 응답 수신 시 호출될 델리게이트입니다.
	void RequestQuestSpeak(int32 SpeakStep, const FString& WavFilePath, FResponseQuestSpeakDelegate InDelegate);

	/// @brief 게임 결과를 요청합니다.
	/// @param InDelegate [in] 응답 수신 시 호출될 델리게이트입니다.
	void RequestGameResult(FResponseGameResultDelegate InDelegate);

*/
	
private:
	/// @brief 공통 네트워크 로그 메시지를 출력합니다.
	/// @param InLogType [in] 로그 타입입니다.
	/// @param URL [in] 요청 URL입니다.
	/// @param Body [in] 요청 본문입니다 (선택사항).
	static void LogNetwork( ENetworkLogType InLogType, const FString& URL, const FString& Body = TEXT(""));

	/// @brief 요청 타입에 맞는 로그 접두사를 반환합니다.
	static const TCHAR* GetLogPrefix(ENetworkLogType InLogType);

	static bool IsResSuccess(const int InCode);

	/// @brief 네트워크 에러 팝업을 표시합니다.
	/// @param ResponseCode [in] HTTP 응답 코드입니다.
	/// @param ResponseContent [in] HTTP 응답 본문입니다.
	void ShowNetworkErrorPopup(int32 ResponseCode, const FString& ResponseContent);
	
	/// @brief 네트워크 처리 대기 카운트를 갱신하고 UI에 통보합니다.
	/// @param Value [in] 증감할 값입니다.
	void AddNetworkWaitCount(int Value);

	
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> SetupHttpRequest( const FString& Url, const FString& Verb );

private:
	/// @brief 처리 중인 요청 수를 집계하는 카운터입니다.
	int NetworkWaitCount = 0;

	/// @brief 인증 토큰 (로그인 후 저장)
	FString access_token;
};
