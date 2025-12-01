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

    /// @brief Scenario 조회 엔드포인트입니다. GET /scenario/{index}/{dificulity}/{lang}
    static FString scenario = FString("/scenario");
    
    /// @brief OCR 텍스트 추출 엔드포인트입니다. POST /writes/ocr/extract
    static FString writes_ocr_extract = FString("/writes/ocr/extract");

	static FString speakings_questions = FString("/speakings/questions");

	static FString interview_hello = FString("/interview/hello");
	static FString interview_answer = FString("/interview/answer/post");
    
    /*

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
	*/
}




USTRUCT(BlueprintType)
struct FInterviewQuestionData
{
	GENERATED_BODY()

	// id : 126
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Id = 0;

	// type_code : 1
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 TypeCode = 0;

	// "eng":"What do you do on weekends?"
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString Eng;

	// "kor":"주말에 보통 무엇을 합니까?"
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString Kor;

	// "eng_key":"weekends"
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString EngKey;

	// "kor_key":"주말"
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString KorKey;

	// "created_at":"2025-11-28T15:51:30.017204"
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString CreatedAt;
};

/// @brief Read 퀘스트 정보 구조체입니다.
USTRUCT(BlueprintType)
struct FInterviewData
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, Category = "KLingo")
	TArray<FInterviewQuestionData> Question;
};



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

	TArray<FWordData> GetSplitData() const;	
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
// Read Quest Role and Result Structures
// =================================================================================

/// @brief Read 퀘스트의 플레이어 결과 기록입니다.
USTRUCT(BlueprintType)
struct FReadQuestResult
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, Category = "Quest")
	bool bSuccess = false;  // 성공 여부

	UPROPERTY(BlueprintReadWrite, Category = "Quest")
	float RemainTime = 0.f;  // 남은 시간

	UPROPERTY(BlueprintReadWrite, Category = "Quest")
	int32 AttemptCount = 0;  // 시도 횟수

	UPROPERTY(BlueprintReadWrite, Category = "Quest")
	FString SelectedSymbol;  // 선택한 심볼

	UPROPERTY(BlueprintReadWrite, Category = "Quest")
	FString SelectedColor;   // 선택한 색상
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


// =================================================================================
// Scenario API Structures
// =================================================================================

/// @brief 단어 정보 구조체                                                                                                                                                                                                       
USTRUCT(BlueprintType)
struct FWordInfo                                                                                                                                                                                                                  
{                                                                                                                                                                                                                                 
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, Category = "Word")
	FString name;

	UPROPERTY(BlueprintReadWrite, Category = "Word")
	FString code;
};

/// @brief Scenario 타겟 데이터입니다.
USTRUCT(BlueprintType)
struct FScenarioTargetData
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, Category = "Scenario")
	FWordInfo word1;

	UPROPERTY(BlueprintReadWrite, Category = "Scenario")
	FWordInfo word2;
};

/// @brief Scenario 응답 델리게이트입니다.
DECLARE_DELEGATE_TwoParams(FResponseScenarioDelegate, FResponseScenario&, bool);
/// @brief Scenario 응답 구조체입니다.
USTRUCT(BlueprintType)
struct FResponseScenario
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, Category = "Scenario")
	int32 index = 0;

	UPROPERTY(BlueprintReadWrite, Category = "Scenario")
	int32 dificulity = 0;

	UPROPERTY(BlueprintReadWrite, Category = "Scenario")
	TArray<FScenarioTargetData> target_data;

	UPROPERTY(BlueprintReadWrite, Category = "Scenario")
	int32 correct_answer_index = 0;

	UPROPERTY(BlueprintReadWrite, Category = "Scenario")
	FWordData word_data1;

	UPROPERTY(BlueprintReadWrite, Category = "Scenario")
	FWordData word_data2;

	UPROPERTY(BlueprintReadWrite, Category = "Scenario")
	FWordData full_data;

	/// @brief HTTP 응답을 파싱해 구조체를 채웁니다.
	void SetFromHttpResponse(const TSharedPtr<class IHttpResponse, ESPMode::ThreadSafe>& Response);

	/// @brief 디버그 로그에 응답 내용을 출력합니다.
	void PrintData() const;

	TArray<FString> GetWord1List() const;
	TArray<FString> GetWord2List() const;
};

// =================================================================================
// OCR Extract API Structures
// =================================================================================

/// @brief OCR Extract 응답 델리게이트입니다.
DECLARE_DELEGATE_TwoParams(FResponseOcrExtractDelegate, FResponseOcrExtract&, bool);
/// @brief OCR Extract 응답 구조체입니다.
USTRUCT(BlueprintType)
struct FResponseOcrExtract
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, Category = "OCR")
	bool success = false;

	UPROPERTY(BlueprintReadWrite, Category = "OCR")
	FString extracted_text;

	/// @brief HTTP 응답을 파싱해 구조체를 채웁니다.
	void SetFromHttpResponse(const TSharedPtr<class IHttpResponse, ESPMode::ThreadSafe>& Response);

	/// @brief 디버그 로그에 응답 내용을 출력합니다.
	void PrintData() const;
};

// =================================================================================
// Speaking Questions API Structures (Updated)
// =================================================================================

/// @brief Speaking Questions 응답 델리게이트입니다.
DECLARE_DELEGATE_TwoParams(FResponseSpeakingQuestionsDelegate, FResponseSpeakingQuestions&, bool);
/// @brief Speaking Questions 응답 구조체입니다.
USTRUCT(BlueprintType)
struct FResponseSpeakingQuestions
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, Category = "Speaking")
	FString answer;

	/// @brief HTTP 응답을 파싱해 구조체를 채웁니다.
	void SetFromHttpResponse(const TSharedPtr<class IHttpResponse, ESPMode::ThreadSafe>& Response);

	/// @brief 디버그 로그에 응답 내용을 출력합니다.
	void PrintData() const;
};


DECLARE_DELEGATE_TwoParams(FResponseInterviewHelloDelegate, FResponseInterviewHello&, bool);
USTRUCT(BlueprintType)
struct FResponseInterviewHello
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, Category = "Interview")
	TArray<FInterviewQuestionData> Questions;

	/// @brief HTTP 응답을 파싱해 구조체를 채웁니다.
	void SetFromHttpResponse(const TSharedPtr<class IHttpResponse, ESPMode::ThreadSafe>& Response);

	/// @brief 디버그 로그에 응답 내용을 출력합니다.
	void PrintData() const;
};



USTRUCT(BlueprintType)
struct FInterviewAnswerData
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
	int interview_id;

	UPROPERTY(BlueprintReadWrite)
	FString answer;

	UPROPERTY(BlueprintReadWrite)
	int user_id;
};

USTRUCT(BlueprintType)
struct FRequestInterviewAnswer
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, Category = "Interview")
	TArray<FInterviewAnswerData> answer;

	/// @brief 구조체를 JSON 문자열로 변환합니다.
	bool ToJsonString(FString& OutJson) const;
};

DECLARE_DELEGATE_TwoParams(FResponseInterviewAnswerDelegate, FResponseInterviewAnswer&, bool);
USTRUCT(BlueprintType)
struct FResponseInterviewAnswer
{
	GENERATED_BODY()

	/// @brief HTTP 응답을 파싱해 구조체를 채웁니다.
	void SetFromHttpResponse(const TSharedPtr<class IHttpResponse, ESPMode::ThreadSafe>& Response);

	/// @brief 디버그 로그에 응답 내용을 출력합니다.
	void PrintData() const;
};


/*

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
*/