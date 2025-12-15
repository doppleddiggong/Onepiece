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
	
	void RequestListenAudio(const FString& AudioText, FResponseListenAudioDelegate InDelegate);
	void RequestSpeakingJudges( const FString& Question, const FString& AudioPath, FResponseSpeakingJudesDelegate InDelegate);

	void RequestInterviewHello(FResponseInterviewHelloDelegate InDelegate);
	void RequestInterviewAnswer( const FRequestInterviewAnswer& Answer, FResponseInterviewAnswerDelegate InDelegate);

	void RequestReadScenario(FResponseReadScenarioDelegate InDelegate);
	void RequestReadResult(const FRequestReadResult& Result, FResponseReadResultDelegate InDelegate);

	void RequestListenScenario(FResponseListenScenarioDelegate InDelegate);
	void RequestListenResult(const FRequestListenResult& Result, FResponseListenResultDelegate InDelegate);

	void RequestWriteQuestions(FResponseWriteQuestionDelegate InDelegate);
	void RequestWriteSubmit(const TArray<FString>& ImagePathArray, const TArray<FWriteWordData>& QuestionArray,  TArray<FString> InTargetText, FResponseWriteSubmitDelegate InDelegate);

	void RequestSpeakScenario(FResponseSpeakScenarioDelegate InDelegate);

	/// @brief Evaluation 결과를 조회합니다. GET /evaluations/rooms/{room_id}
	void RequestEvaluationResult(int32 RoomId, FResponseEvaluationResultDelegate InDelegate);
	
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
