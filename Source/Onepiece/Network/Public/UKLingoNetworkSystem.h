// Copyright (c) 2025 Doppleddiggong. All rights reserved.
// Unauthorized copying, modification, or distribution of this file,
// via any medium is strictly prohibited. Proprietary and confidential.

/**
 * @file UKLingoNetworkSystem.h
 * @brief KLingo API 요청을 담당하는 서브시스템을 선언합니다.
 */
#pragma once

#include "CoreMinimal.h"
#include "NetworkData.h"
#include "Macro.h"
#include "ENetworkLogType.h"
#include "Interfaces/IHttpRequest.h"
#include "Interfaces/IHttpResponse.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "UKLingoNetworkSystem.generated.h"

/**
 * @brief KLingo 서버와의 HTTP 요청을 중재하는 게임 인스턴스 서브시스템입니다.
 *
 * 학습/퀘스트/채팅 등 여러 API 요청의 공통 진입점으로 사용됩니다.
 * 요청 처리 중 로딩 UI 및 오류 팝업을 관리하며, 응답은 델리게이트로 전달됩니다.
 * 사용 위치: 네트워크 요청이 필요한 게임플레이/학습 UI 전반.
 */
UCLASS()
class ONEPIECE_API UKLingoNetworkSystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	DEFINE_SUBSYSTEM_GETTER_INLINE(UKLingoNetworkSystem);

	/**
	 * @brief HTTP 모듈 초기화 및 네트워크 대기 카운터를 설정합니다.
	 *
	 * 서브시스템 시작 시 요청 상태 추적을 준비하기 위한 초기화 단계입니다.
	 * 사이드이펙트: 내부 카운터 및 요청 대기 상태를 초기화합니다.
	 */
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	/**
	 * @brief 등록된 델리게이트를 정리하고 서브시스템을 종료합니다.
	 *
	 * 잔여 네트워크 요청 상태를 정리해 안전하게 종료합니다.
	 * 사이드이펙트: 내부 상태를 해제하고 UI 대기 상태를 정리합니다.
	 */
	virtual void Deinitialize() override;

	/**
	 * @brief 사용자 등록 요청을 전송합니다.
	 *
	 * 신규 사용자 생성 플로우에서 계정을 생성하기 위해 사용됩니다.
	 * @param UserName [in] 등록할 사용자 이름입니다.
	 * @param InDelegate [in] 응답 콜백입니다.
	 * 사이드이펙트: 네트워크 요청을 전송하고 로딩 UI 상태를 갱신합니다.
	 */
	void RequestUserRegister(const FString& UserName, FResponseUserRegisterDelegate InDelegate);

	/**
	 * @brief 사용자 토큰 발급 요청을 전송합니다.
	 *
	 * 인증 토큰을 발급받아 이후 요청의 권한을 확보하기 위해 사용됩니다.
	 * @param UserName [in] 토큰 발급 대상 사용자 이름입니다.
	 * @param InDelegate [in] 응답 콜백입니다.
	 * 사이드이펙트: 네트워크 요청을 전송하고 로딩 UI 상태를 갱신합니다.
	 */
	void RequestUserToken(const FString& UserName, FResponseUserTokenDelegate InDelegate);

	/**
	 * @brief 현재 사용자 정보를 조회합니다.
	 *
	 * 토큰 기반 인증이 유효한지 검증하는 데 사용됩니다.
	 * @param InDelegate [in] 응답 콜백입니다.
	 * 사이드이펙트: 네트워크 요청을 전송하고 로딩 UI 상태를 갱신합니다.
	 */
	void RequestUserMe(FResponseUserMeDelegate InDelegate);
	
	/**
	 * @brief Listen 음성 생성 요청을 전송합니다.
	 *
	 * 텍스트를 음성으로 변환해 듣기 학습에 사용하는 요청입니다.
	 * @param AudioText [in] 음성으로 변환할 텍스트입니다.
	 * @param InDelegate [in] 응답 콜백입니다.
	 * 사이드이펙트: 네트워크 요청을 전송하고 로딩 UI 상태를 갱신합니다.
	 */
	void RequestListenAudio(const FString& AudioText, FResponseListenAudioDelegate InDelegate);

	/**
	 * @brief Speaking 평가 요청을 전송합니다.
	 *
	 * 질문 텍스트와 사용자의 음성 파일을 제출해 발음 평가를 요청합니다.
	 * @param Question [in] 평가 기준 질문 텍스트입니다.
	 * @param AudioPath [in] 업로드할 음성 파일 경로입니다.
	 * @param InDelegate [in] 응답 콜백입니다.
	 * 사이드이펙트: 네트워크 요청을 전송하고 로딩 UI 상태를 갱신합니다.
	 */
	void RequestSpeakingJudges( const FString& Question, const FString& AudioPath, FResponseSpeakingJudesDelegate InDelegate);

	/**
	 * @brief 인터뷰 시작 요청을 전송합니다.
	 *
	 * 인터뷰 시나리오를 시작하기 위한 진입 요청입니다.
	 * @param InDelegate [in] 응답 콜백입니다.
	 * 사이드이펙트: 네트워크 요청을 전송하고 로딩 UI 상태를 갱신합니다.
	 */
	void RequestInterviewHello(FResponseInterviewHelloDelegate InDelegate);

	/**
	 * @brief 인터뷰 답변 제출 요청을 전송합니다.
	 *
	 * 사용자 답변을 서버로 전송하여 평가/저장을 수행합니다.
	 * @param Answer [in] 제출할 인터뷰 답변 데이터입니다.
	 * @param InDelegate [in] 응답 콜백입니다.
	 * 사이드이펙트: 네트워크 요청을 전송하고 로딩 UI 상태를 갱신합니다.
	 */
	void RequestInterviewAnswer( const FRequestInterviewAnswer& Answer, FResponseInterviewAnswerDelegate InDelegate);

	/**
	 * @brief Read 시나리오 조회 요청을 전송합니다.
	 *
	 * 읽기 학습 단계의 시나리오 데이터를 받아오기 위한 호출입니다.
	 * @param InDelegate [in] 응답 콜백입니다.
	 * 사이드이펙트: 네트워크 요청을 전송하고 로딩 UI 상태를 갱신합니다.
	 */
	void RequestReadScenario(FResponseReadScenarioDelegate InDelegate);

	/**
	 * @brief Read 결과 제출 요청을 전송합니다.
	 *
	 * 읽기 학습 결과를 서버에 기록하기 위해 호출합니다.
	 * @param Result [in] 제출할 결과 데이터입니다.
	 * @param InDelegate [in] 응답 콜백입니다.
	 * 사이드이펙트: 네트워크 요청을 전송하고 로딩 UI 상태를 갱신합니다.
	 */
	void RequestReadResult(const FRequestReadResult& Result, FResponseReadResultDelegate InDelegate);

	/**
	 * @brief Listen 시나리오 조회 요청을 전송합니다.
	 *
	 * 듣기 학습 단계의 시나리오 데이터를 받아오기 위한 호출입니다.
	 * @param InDelegate [in] 응답 콜백입니다.
	 * 사이드이펙트: 네트워크 요청을 전송하고 로딩 UI 상태를 갱신합니다.
	 */
	void RequestListenScenario(FResponseListenScenarioDelegate InDelegate);

	/**
	 * @brief Listen 결과 제출 요청을 전송합니다.
	 *
	 * 듣기 학습 결과를 서버에 기록하기 위해 호출합니다.
	 * @param Result [in] 제출할 결과 데이터입니다.
	 * @param InDelegate [in] 응답 콜백입니다.
	 * 사이드이펙트: 네트워크 요청을 전송하고 로딩 UI 상태를 갱신합니다.
	 */
	void RequestListenResult(const FRequestListenResult& Result, FResponseListenResultDelegate InDelegate);

	/**
	 * @brief Write 문제 목록을 요청합니다.
	 *
	 * 쓰기 학습 단계의 문제 리스트를 서버에서 받아옵니다.
	 * @param InDelegate [in] 응답 콜백입니다.
	 * 사이드이펙트: 네트워크 요청을 전송하고 로딩 UI 상태를 갱신합니다.
	 */
	void RequestWriteQuestions(FResponseWriteQuestionDelegate InDelegate);

	/**
	 * @brief Write 제출 요청을 전송합니다.
	 *
	 * 이미지 경로 목록과 타겟 텍스트를 서버로 제출해 OCR/채점 처리에 사용합니다.
	 * @param ImagePathArray [in] 업로드할 이미지 파일 경로 배열입니다.
	 * @param InTargetText [in] 비교 대상 텍스트 배열입니다.
	 * @param InDelegate [in] 응답 콜백입니다.
	 * 사이드이펙트: 네트워크 요청을 전송하고 로딩 UI 상태를 갱신합니다.
	 */
	void RequestWriteSubmit(const TArray<FString>& ImagePathArray, TArray<FString> InTargetText, FResponseWriteSubmitDelegate InDelegate);

	/**
	 * @brief Write 결과 제출 요청을 전송합니다.
	 *
	 * 쓰기 학습 결과를 서버에 기록하기 위해 호출합니다.
	 * @param Result [in] 제출할 결과 데이터입니다.
	 * @param InDelegate [in] 응답 콜백입니다.
	 * 사이드이펙트: 네트워크 요청을 전송하고 로딩 UI 상태를 갱신합니다.
	 */
	void RequestWriteResult(const FRequestWriteResult& Result, FResponseWriteResultDelegate InDelegate);

	/**
	 * @brief Speak 시나리오 조회 요청을 전송합니다.
	 *
	 * 말하기 학습 단계의 시나리오 데이터를 받아오기 위한 호출입니다.
	 * @param InDelegate [in] 응답 콜백입니다.
	 * 사이드이펙트: 네트워크 요청을 전송하고 로딩 UI 상태를 갱신합니다.
	 */
	void RequestSpeakScenario(FResponseSpeakScenarioDelegate InDelegate);

	/**
	 * @brief Speak 결과 제출 요청을 전송합니다.
	 *
	 * 말하기 학습 결과를 서버에 기록하기 위해 호출합니다.
	 * @param Result [in] 제출할 결과 데이터입니다.
	 * @param InDelegate [in] 응답 콜백입니다.
	 * 사이드이펙트: 네트워크 요청을 전송하고 로딩 UI 상태를 갱신합니다.
	 */
	void RequestSpeakResult(const FRequestSpeakResult& Result, FResponseSpeakResultDelegate InDelegate);

	/**
	 * @brief Evaluation 결과를 조회합니다. GET /evaluations/rooms/{room_id}
	 *
	 * 평가 결과를 UI에 표시하기 위해 서버에서 데이터를 가져옵니다.
	 * @param RoomId [in] 평가 룸 ID입니다.
	 * @param InDelegate [in] 응답 콜백입니다.
	 * 사이드이펙트: 네트워크 요청을 전송하고 로딩 UI 상태를 갱신합니다.
	 */
	void RequestEvaluationResult(int32 RoomId, FResponseEvaluationResultDelegate InDelegate);

	/**
	 * @brief Chat 답변을 요청합니다 (텍스트 질문). POST /chats/answers
	 *
	 * 대화 맥락과 질문을 제출해 답변을 생성합니다.
	 * @param Context [in] 대화 컨텍스트입니다.
	 * @param Question [in] 사용자 질문입니다.
	 * @param InDelegate [in] 응답 콜백입니다.
	 * 사이드이펙트: 네트워크 요청을 전송하고 로딩 UI 상태를 갱신합니다.
	 */
	void RequestChatQuestion(const FString& Context, const FString& Question, FResponseChatAnswersDelegate InDelegate);

	/**
	 * @brief Chat 답변을 요청합니다 (음성 질문). POST /chats/answers
	 *
	 * 음성 파일을 업로드해 답변을 생성합니다.
	 * @param Context [in] 대화 컨텍스트입니다.
	 * @param AudioPath [in] 음성 파일 경로입니다.
	 * @param InDelegate [in] 응답 콜백입니다.
	 * 사이드이펙트: 네트워크 요청을 전송하고 로딩 UI 상태를 갱신합니다.
	 */
	void RequestChatAudio(const FString& Context, const FString& AudioPath, FResponseChatAnswersDelegate InDelegate);

	/**
	 * @brief Daily Chat 답변을 요청합니다 (텍스트 질문). POST /chats/dailys
	 *
	 * 데일리 학습용 대화 컨텍스트로 답변을 생성합니다.
	 * @param Context [in] 대화 컨텍스트입니다.
	 * @param Question [in] 사용자 질문입니다.
	 * @param InDelegate [in] 응답 콜백입니다.
	 * 사이드이펙트: 네트워크 요청을 전송하고 로딩 UI 상태를 갱신합니다.
	 */
	void RequestDailyQuestion(const FString& Context, const FString& Question, FResponseChatDailysDelegate InDelegate);

	/**
	 * @brief Daily Chat 답변을 요청합니다 (음성 질문). POST /chats/dailys
	 *
	 * 데일리 학습용 음성 질문을 업로드해 답변을 생성합니다.
	 * @param Context [in] 대화 컨텍스트입니다.
	 * @param AudioPath [in] 음성 파일 경로입니다.
	 * @param InDelegate [in] 응답 콜백입니다.
	 * 사이드이펙트: 네트워크 요청을 전송하고 로딩 UI 상태를 갱신합니다.
	 */
	void RequestDailyAudio(const FString& Context, const FString& AudioPath, FResponseChatDailysDelegate InDelegate);

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

	/// @brief LoadingCircle을 표시하거나 숨깁니다.
	/// @param bShow [in] true면 표시, false면 숨김
	void ShowLoadingCircle(bool bShow);


	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> SetupHttpRequest( const FString& Url, const FString& Verb );

private:
	/// @brief 처리 중인 요청 수를 집계하는 카운터입니다.
	int NetworkWaitCount = 0;

	/// @brief 인증 토큰 (로그인 후 저장)
	FString access_token;
};
