// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.

/// @file NetworkData.h
/// @brief 네트워크 요청과 응답에 사용되는 구조체 및 설정을 정의합니다.
#pragma once

#include "CoreMinimal.h"
#include "UCustomNetworkSettings.h"
#include "Templates/SharedPointer.h"
#include "NetworkData.generated.h"

// =================================================================================
// Network Configuration
// 서버 주소와 포트를 설정합니다.
// =================================================================================
namespace NetworkConfig
{
    /// @brief 현재 서버 모드에 맞는 HTTP 엔드포인트 URL을 생성합니다.
    /// @param Endpoint [in] 호출할 상대 경로입니다.
    /// @return 모드와 포트가 적용된 전체 URL입니다.
    static FString GetFullUrl(const FString& Endpoint)
    {
        const EServerMode Mode = UCustomNetworkSettings::GetCurrentServerMode();
        const FServerConfig& Config = GetDefault<UCustomNetworkSettings>()->GetConfig(Mode);
        return Config.GetFullUrl(Endpoint);
    }

    /// @brief 현재 서버 모드에서 사용할 WebSocket 주소를 반환합니다.
    /// @return ws(s):// 형식의 소켓 접속 URL입니다.
    static FString GetSocketURL()
    {
        const EServerMode Mode = UCustomNetworkSettings::GetCurrentServerMode();
        const FServerConfig& Config = GetDefault<UCustomNetworkSettings>()->GetConfig(Mode);
        return Config.WebSocketUrl;
    }
}

namespace RequestAPI
{
    /// @brief 서버 상태 확인 엔드포인트입니다.
    static FString Health = FString("/health");


    static FString users_register = FString("/users/register");
    static FString users_token = FString("/users/token");
    static FString users_me = FString("/users/me");

    static FString speakings_questions = FString("/speaking/questions");



    /// @brief KLingo 로그인 엔드포인트입니다.
    static FString Login = FString("/Login");

    /// @brief KLingo 사용자 생성 엔드포인트입니다.
    static FString CreateUser = FString("/CreateUserInfo");

    /// @brief KLingo 인터뷰 요청 엔드포인트입니다.
    static FString Interview = FString("/RequestInterview");

    /// @brief KLingo 인터뷰 제출 엔드포인트입니다.
    static FString SubmitInterview = FString("/SubmitInterviewAnswer");

    /// @brief KLingo 게임 시작 엔드포인트입니다.
    static FString StartGame = FString("/StartGame");

    /// @brief KLingo 게임 로그인 엔드포인트입니다.
    static FString GameLogin = FString("/GameLogin");

    /// @brief KLingo 퀘스트 답변 엔드포인트입니다.
    static FString QuestAnswer = FString("/QuestAnswer");

    /// @brief KLingo 퀘스트 실패 엔드포인트입니다.
    static FString QuestFail = FString("/QuestFail");

    /// @brief KLingo Write 퀘스트 엔드포인트입니다.
    static FString QuestWrite = FString("/QuestWrite");

    /// @brief KLingo Speak 퀘스트 엔드포인트입니다.
    static FString QuestSpeak = FString("/QuestSpeak");

    /// @brief KLingo 게임 결과 엔드포인트입니다.
    static FString GameResult = FString("/GameResult");



	/// @brief 음성-텍스트-음성을 통합 처리하는 ASK 엔드포인트입니다.
	static FString ASK = FString("/ask");
}



// =================================================================================
// Common Data Structures
// =================================================================================

/// @brief 플레이어 정보 구조체입니다.
USTRUCT(BlueprintType)
struct FPlayerInfo
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, Category = "PlayerInfo")
	int32 UserIndex = 0;

	UPROPERTY(BlueprintReadWrite, Category = "PlayerInfo")
	FString Nickname;

	UPROPERTY(BlueprintReadWrite, Category = "PlayerInfo")
	int32 Gold = 0;

	UPROPERTY(BlueprintReadWrite, Category = "PlayerInfo")
	int32 Character = -1;

	UPROPERTY(BlueprintReadWrite, Category = "PlayerInfo")
	int32 CharacterColor = -1;

	UPROPERTY(BlueprintReadWrite, Category = "PlayerInfo")
	FString UnlockEquip;

	UPROPERTY(BlueprintReadWrite, Category = "PlayerInfo")
	bool bLogin = false;
};

/// @brief 단어 데이터 구조체입니다.
USTRUCT(BlueprintType)
struct FWordData
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, Category = "Word")
	FString Kor;

	UPROPERTY(BlueprintReadWrite, Category = "Word")
	FString Eng;

	UPROPERTY(BlueprintReadWrite, Category = "Word")
	FString Pronunciation;
};

// =================================================================================
// Read Quest Structures
// =================================================================================

/// @brief Read 퀘스트 타겟 데이터입니다.
USTRUCT(BlueprintType)
struct FReadTargetData
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, Category = "KLingo")
	FString FindSymbol;

	UPROPERTY(BlueprintReadWrite, Category = "KLingo")
	FString FindColor;
};

/// @brief Read 퀘스트 정보 구조체입니다.
USTRUCT(BlueprintType)
struct FQuestReadInfo
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, Category = "KLingo")
	int32 ReadIndex = 0;

	UPROPERTY(BlueprintReadWrite, Category = "KLingo")
	int32 Difficulty = 0;

	UPROPERTY(BlueprintReadWrite, Category = "KLingo")
	TArray<FReadTargetData> QuestTarget;

	UPROPERTY(BlueprintReadWrite, Category = "KLingo")
	int32 CorrectAnswerIndex = 0;

	UPROPERTY(BlueprintReadWrite, Category = "KLingo")
	FWordData WordData1;

	UPROPERTY(BlueprintReadWrite, Category = "KLingo")
	FWordData WordData2;

	UPROPERTY(BlueprintReadWrite, Category = "KLingo")
	FString FullData;
};

// =================================================================================
// Listen Quest Structures
// =================================================================================

/// @brief Listen 퀘스트 타겟 데이터입니다.
USTRUCT(BlueprintType)
struct FListenTargetData
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, Category = "KLingo")
	FString FindTarget1;

	UPROPERTY(BlueprintReadWrite, Category = "KLingo")
	FString FindTarget2;
};

/// @brief Listen 퀘스트 정보 구조체입니다.
USTRUCT(BlueprintType)
struct FQuestListenInfo
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, Category = "KLingo")
	int32 ListenIndex = 0;

	UPROPERTY(BlueprintReadWrite, Category = "KLingo")
	int32 Difficulty = 0;

	UPROPERTY(BlueprintReadWrite, Category = "KLingo")
	TArray<FListenTargetData> QuestTarget;

	UPROPERTY(BlueprintReadWrite, Category = "KLingo")
	int32 CorrectAnswerIndex = 0;

	UPROPERTY(BlueprintReadWrite, Category = "KLingo")
	FWordData WordData1;

	UPROPERTY(BlueprintReadWrite, Category = "KLingo")
	FWordData WordData2;

	UPROPERTY(BlueprintReadWrite, Category = "KLingo")
	FString FullData;
};

// =================================================================================
// Write Quest Structures
// =================================================================================

/// @brief Write 퀘스트 타겟 데이터입니다.
USTRUCT(BlueprintType)
struct FWriteTargetData
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, Category = "KLingo")
	FString TextureData;
};

/// @brief Write 교육 데이터 구조체입니다.
USTRUCT(BlueprintType)
struct FWriteTeachData
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, Category = "KLingo")
	int32 Index = 0;

	UPROPERTY(BlueprintReadWrite, Category = "KLingo")
	FString TeachString;
};

/// @brief Write 퀘스트 정보 구조체입니다.
USTRUCT(BlueprintType)
struct FQuestWriteInfo
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, Category = "KLingo")
	int32 WriteIndex = 0;

	UPROPERTY(BlueprintReadWrite, Category = "KLingo")
	int32 Difficulty = 0;

	UPROPERTY(BlueprintReadWrite, Category = "KLingo")
	TArray<FWriteTargetData> QuestTarget;
};

// =================================================================================
// Speak Quest Structures
// =================================================================================

/// @brief Speak 퀘스트 정보 구조체입니다.
USTRUCT(BlueprintType)
struct FQuestSpeakInfo
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, Category = "KLingo")
	int32 SpeakIndex = 0;

	UPROPERTY(BlueprintReadWrite, Category = "KLingo")
	int32 Difficulty = 0;

	UPROPERTY(BlueprintReadWrite, Category = "KLingo")
	TArray<FString> AnswerPlayer;

	UPROPERTY(BlueprintReadWrite, Category = "KLingo")
	TArray<FString> QuestTarget;
};

// =================================================================================
// Response Structures
// =================================================================================
/// @brief 헬스 체크 응답을 전달하는 델리게이트입니다.
DECLARE_DELEGATE_TwoParams(FResponseHealthDelegate, FResponseHealth&, bool);
USTRUCT(BlueprintType)
struct FResponseHealth
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite)
    int32 status = 0;

    /// @brief HTTP 응답을 파싱해 상태 정보를 갱신합니다.
    void SetFromHttpResponse(const TSharedPtr<class IHttpResponse, ESPMode::ThreadSafe>& Response);

    /// @brief 디버그 로그에 응답 내용을 출력합니다.
    void PrintData();
};



DECLARE_DELEGATE_TwoParams(FResponseUserRegisterDelegate, FResponseUserRegister&, bool);
USTRUCT(BlueprintType)
struct FResponseUserRegister
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
	int32 id = 0;

	UPROPERTY(BlueprintReadWrite)
	FString username;

	UPROPERTY(BlueprintReadWrite)
	FString email;

	UPROPERTY(BlueprintReadWrite)
	bool is_active;
	
	/// @brief HTTP 응답을 파싱해 상태 정보를 갱신합니다.
	void SetFromHttpResponse(const TSharedPtr<class IHttpResponse, ESPMode::ThreadSafe>& Response);

	/// @brief 디버그 로그에 응답 내용을 출력합니다.
	void PrintData() const;
};


DECLARE_DELEGATE_TwoParams(FResponseUserTokenDelegate, FResponseUserToken&, bool);
USTRUCT(BlueprintType)
struct FResponseUserToken
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, Category = "Token")
	FString access_token;

	/// @brief HTTP 응답을 파싱해 상태 정보를 갱신합니다.
	void SetFromHttpResponse(const TSharedPtr<class IHttpResponse, ESPMode::ThreadSafe>& Response);

	/// @brief 디버그 로그에 응답 내용을 출력합니다.
	void PrintData() const;
};


DECLARE_DELEGATE_TwoParams(FResponseUserMeDelegate, FResponseUserMe&, bool);
USTRUCT(BlueprintType)
struct FResponseUserMe
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, Category = "Me")
	FString detail;

	/// @brief HTTP 응답을 파싱해 상태 정보를 갱신합니다.
	void SetFromHttpResponse(const TSharedPtr<class IHttpResponse, ESPMode::ThreadSafe>& Response);

	/// @brief 디버그 로그에 응답 내용을 출력합니다.
	void PrintData() const;
};


DECLARE_DELEGATE_TwoParams(FResponseSpeakingsQuestionsDelegate, FResponseSpeakingsQuestions&, bool);
USTRUCT(BlueprintType)
struct FResponseSpeakingsQuestions
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, Category = "Me")
	FString detail;

	/// @brief HTTP 응답을 파싱해 상태 정보를 갱신합니다.
	void SetFromHttpResponse(const TSharedPtr<class IHttpResponse, ESPMode::ThreadSafe>& Response);

	/// @brief 디버그 로그에 응답 내용을 출력합니다.
	void PrintData() const;
};



/// @brief 로그인 응답 델리게이트입니다.
DECLARE_DELEGATE_TwoParams(FResponseLoginDelegate, FResponseLogin&, bool);
/// @brief 로그인 응답 구조체입니다.
USTRUCT(BlueprintType)
struct FResponseLogin
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, Category = "Login")
	FString Token;

	UPROPERTY(BlueprintReadWrite, Category = "Login")
	FPlayerInfo PlayerInfo;

	/// @brief HTTP 응답을 파싱해 구조체를 채웁니다.
	void SetFromHttpResponse(const TSharedPtr<class IHttpResponse, ESPMode::ThreadSafe>& Response);

	/// @brief 디버그 로그에 응답 내용을 출력합니다.
	void PrintData() const;
};

/// @brief 사용자 생성 응답 델리게이트입니다.
DECLARE_DELEGATE_TwoParams(FResponseCreateUserDelegate, FResponseCreateUser&, bool);
/// @brief 사용자 생성 응답 구조체입니다.
USTRUCT(BlueprintType)
struct FResponseCreateUser
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, Category = "CreateUser")
	bool bSuccess = false;

	UPROPERTY(BlueprintReadWrite, Category = "CreateUser")
	FString Message;

	void SetFromHttpResponse(const TSharedPtr<class IHttpResponse, ESPMode::ThreadSafe>& Response);
	void PrintData() const;
};

/// @brief 인터뷰 응답 델리게이트입니다.
DECLARE_DELEGATE_TwoParams(FResponseInterviewDelegate, FResponseInterview&, bool);
/// @brief 인터뷰 응답 구조체입니다.
USTRUCT(BlueprintType)
struct FResponseInterview
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, Category = "Interview")
	TArray<FString> Questions;

	void SetFromHttpResponse(const TSharedPtr<class IHttpResponse, ESPMode::ThreadSafe>& Response);
	void PrintData() const;
};

/// @brief 게임 시작 응답 델리게이트입니다.
DECLARE_DELEGATE_TwoParams(FResponseStartGameDelegate, FResponseStartGame&, bool);
/// @brief 게임 시작 응답 구조체입니다.
USTRUCT(BlueprintType)
struct FResponseStartGame
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, Category = "StartGame")
	FQuestReadInfo QuestRead;

	UPROPERTY(BlueprintReadWrite, Category = "StartGame")
	FQuestListenInfo QuestListen;

	UPROPERTY(BlueprintReadWrite, Category = "StartGame")
	FQuestWriteInfo QuestWrite;

	UPROPERTY(BlueprintReadWrite, Category = "StartGame")
	FQuestSpeakInfo QuestSpeak;

	void SetFromHttpResponse(const TSharedPtr<class IHttpResponse, ESPMode::ThreadSafe>& Response);
	void PrintData() const;
};

/// @brief 게임 로그인 응답 델리게이트입니다.
DECLARE_DELEGATE_TwoParams(FResponseGameLoginDelegate, FResponseGameLogin&, bool);
/// @brief 게임 로그인 응답 구조체입니다.
USTRUCT(BlueprintType)
struct FResponseGameLogin
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, Category = "GameLogin")
	bool bSuccess = false;

	UPROPERTY(BlueprintReadWrite, Category = "GameLogin")
	FString SessionToken;

	void SetFromHttpResponse(const TSharedPtr<class IHttpResponse, ESPMode::ThreadSafe>& Response);
	void PrintData() const;
};

/// @brief 퀘스트 답변 응답 델리게이트입니다.
DECLARE_DELEGATE_TwoParams(FResponseQuestAnswerDelegate, FResponseQuestAnswer&, bool);
/// @brief 퀘스트 답변 응답 구조체입니다.
USTRUCT(BlueprintType)
struct FResponseQuestAnswer
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, Category = "QuestAnswer")
	int32 Result = 0;

	UPROPERTY(BlueprintReadWrite, Category = "QuestAnswer")
	FString TeachString;

	UPROPERTY(BlueprintReadWrite, Category = "QuestAnswer")
	int32 FailCount = 0;

	void SetFromHttpResponse(const TSharedPtr<class IHttpResponse, ESPMode::ThreadSafe>& Response);
	void PrintData() const;
};


/// @brief Write 퀘스트 응답 델리게이트입니다.
DECLARE_DELEGATE_TwoParams(FResponseQuestWriteDelegate, FResponseQuestWrite&, bool);
/// @brief Write 퀘스트 응답 구조체입니다.
USTRUCT(BlueprintType)
struct FResponseQuestWrite
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, Category = "QuestWrite")
	bool bSuccess = false;

	UPROPERTY(BlueprintReadWrite, Category = "QuestWrite")
	TArray<FWriteTeachData> TeachData;

	void SetFromHttpResponse(const TSharedPtr<class IHttpResponse, ESPMode::ThreadSafe>& Response);
	void PrintData() const;
};

/// @brief Speak 퀘스트 응답 델리게이트입니다.
DECLARE_DELEGATE_TwoParams(FResponseQuestSpeakDelegate, FResponseQuestSpeak&, bool);
/// @brief Speak 퀘스트 응답 구조체입니다.
USTRUCT(BlueprintType)
struct FResponseQuestSpeak
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, Category = "QuestSpeak")
	bool bSuccess = false;

	UPROPERTY(BlueprintReadWrite, Category = "QuestSpeak")
	FString Feedback;

	UPROPERTY(BlueprintReadWrite, Category = "QuestSpeak")
	int32 Score = 0;

	void SetFromHttpResponse(const TSharedPtr<class IHttpResponse, ESPMode::ThreadSafe>& Response);
	void PrintData() const;
};

/// @brief 게임 결과 응답 델리게이트입니다.
DECLARE_DELEGATE_TwoParams(FResponseGameResultDelegate, FResponseGameResult&, bool);
/// @brief 게임 결과 응답 구조체입니다.
USTRUCT(BlueprintType)
struct FResponseGameResult
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, Category = "GameResult")
	FString Grade;

	UPROPERTY(BlueprintReadWrite, Category = "GameResult")
	TArray<int32> Scores;

	void SetFromHttpResponse(const TSharedPtr<class IHttpResponse, ESPMode::ThreadSafe>& Response);
	void PrintData() const;
};














/// @brief 위치 정보를 표현하는 GPT 컨텍스트 요소입니다.
USTRUCT(BlueprintType)
struct FGPTContextLocation
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
	FString name;

	UPROPERTY(BlueprintReadWrite)
	float x = 0.0f;

	UPROPERTY(BlueprintReadWrite)
	float y = 0.0f;

	UPROPERTY(BlueprintReadWrite)
	float z = 0.0f;

	UPROPERTY(BlueprintReadWrite)
	bool bValid = false;

	/// @brief 위치 정보를 초기화합니다.
	void Reset();

	/// @brief 이름과 좌표를 설정하고 유효 상태로 전환합니다.
	/// @param InName [in] 위치를 설명하는 이름입니다.
	/// @param InPosition [in] 월드 좌표입니다.
	void Set(const FString& InName, const FVector& InPosition);

	/// @brief 구조체가 유효한 데이터를 보유하고 있는지 확인합니다.
	bool IsValid() const;

	/// @brief JSON 직렬화를 위한 객체를 생성합니다.
	TSharedPtr<FJsonObject> ToJsonObject() const;
};

/// @brief 주변 건물 정보를 표현하는 GPT 컨텍스트 요소입니다.
USTRUCT(BlueprintType)
struct FGPTContextNearBuilding
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
	FString name;

	UPROPERTY(BlueprintReadWrite)
	float distance = 0.0f;

	UPROPERTY(BlueprintReadWrite)
	bool bValid = false;

	/// @brief 필드를 초기 상태로 되돌립니다.
	void Reset();

	/// @brief 건물 이름과 거리를 설정합니다.
	/// @param InName [in] 건물 고유 이름입니다.
	/// @param InDistanceMeters [in] 플레이어와의 거리(미터)입니다.
	void Set(const FString& InName, float InDistanceMeters);

	/// @brief 데이터가 유효한지 여부를 반환합니다.
	bool IsValid() const;

	/// @brief JSON 직렬화를 위한 객체를 생성합니다.
	TSharedPtr<FJsonObject> ToJsonObject() const;
};

/// @brief 플레이어 주변 맥락을 GPT에 전달하기 위한 전체 컨텍스트입니다.
USTRUCT(BlueprintType)
struct FGPTContext
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
	FGPTContextLocation current_location;

	UPROPERTY(BlueprintReadWrite)
	FGPTContextLocation focused_object;

	UPROPERTY(BlueprintReadWrite)
	TArray<FGPTContextNearBuilding> nearby_buildings;

	/// @brief 내부 데이터를 초기화합니다.
	void Reset();

	/// @brief 어느 필드라도 유효한 정보를 포함하는지 확인합니다.
	bool HasAnyData() const;

	/// @brief 주변 건물 목록에 항목을 추가합니다.
	/// @param InBuilding [in] 추가할 건물 정보입니다.
	void AddNearbyBuilding(const FGPTContextNearBuilding& InBuilding);

	/// @brief JSON 직렬화를 위한 객체를 생성합니다.
	TSharedPtr<FJsonObject> ToJsonObject() const;
};







// --- Ask Endpoint (Integrated Pipeline: STT -> GPT -> TTS) ---
USTRUCT(BlueprintType)
struct FRequestASK
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
	FGPTContext context;

	/// @brief 구조체를 JSON 문자열로 직렬화합니다.
	bool ToJsonString(FString& OutJson) const;
};


USTRUCT(BlueprintType)
struct FRequestGPT
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
	FString user_query;

	UPROPERTY(BlueprintReadWrite)
	FGPTContext context;

	/// @brief 구조체를 JSON 문자열로 변환합니다.
	bool ToJsonString(FString& OutJson) const;
};

/// @brief ASK/GPT 응답을 전달하는 델리게이트입니다.
DECLARE_DELEGATE_TwoParams(FResponseAskDelegate, FResponseAsk&, bool);
USTRUCT(BlueprintType)
struct FResponseAsk
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
	FString transcribed_text;

	UPROPERTY(BlueprintReadWrite)
	FString gpt_response_text;

	UPROPERTY(BlueprintReadWrite)
	TArray<uint8> audio_data;

	/// @brief HTTP 응답을 해석해 필드 값을 채웁니다.
	void SetFromHttpResponse(const TSharedPtr<IHttpResponse, ESPMode::ThreadSafe>& Response);
	/// @brief 응답 데이터를 로그에 출력합니다.
	void PrintData();
};
